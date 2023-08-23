/*
 * Copyright 2021 Lars Pastewka
 *
 * ### MIT license
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef YAMD_MPI_SUPPORT_H
#define YAMD_MPI_SUPPORT_H

#include <mpi.h>

#include <Eigen/Dense>

/*
 * Wrap integer value to interval [0..range-1]. This is the modulo operation.
 */
template <typename T> inline T wrap_to_interval(T value, T range) {
    while (value < 0)
        value += range;
    while (value > range - 1)
        value -= range;
    return value;
}

/*
 * Wrap integer value to a distance on the interval [0..range-1].
 */
template <typename T> inline T wrap_to_distance(T value, T range) {
    T v{value + range / 2};
    return wrap_to_interval(v, range) - range / 2;
}

namespace MPI {
// Ensure finalize is always called
struct init_guard {
    init_guard(int *argc, char ***argv) { MPI_Init(argc, argv); }
    init_guard() : init_guard(nullptr, nullptr) {}
    ~init_guard() { MPI_Finalize(); }
};

/*
 * The following functions convert C++ data types to MPI type identifiers at
 * compile time. Example: MPI_Datatype t_int{mpi_type<int>()};
 */
template <typename T, typename T2 = T> inline decltype(auto) mpi_type() {
    static_assert(std::is_same<T, T2>::value, "T2 is a SFINAE parameter, do not touch");
    static_assert(std::is_same<T, T2>::value and not std::is_same<T, T2>::value,
                  "The type you're trying to map has not been declared.");
    return MPI_LONG;
}

/*
 * Template specialization for type `char`
 */
template <> inline decltype(auto) mpi_type<char>() { return MPI_CHAR; }

/*
 * Template specialization for type `short`
 */
template <> inline decltype(auto) mpi_type<short>() { return MPI_SHORT; }

/*
 * Template specialization for type `int`
 */
template <> inline decltype(auto) mpi_type<int>() { return MPI_INT; }

/*
 * Template specialization for type `long`
 */
template <> inline decltype(auto) mpi_type<long>() { return MPI_LONG; }

/*
 * Template specialization for type `long long`
 */
template <> inline decltype(auto) mpi_type<long long>() { return MPI_LONG_LONG_INT; }

/*
 * Template specialization for type `unsigned char`
 */
template <> inline decltype(auto) mpi_type<unsigned char>() { return MPI_UNSIGNED_CHAR; }

/*
 * Template specialization for type `unsigned short`
 */
template <> inline decltype(auto) mpi_type<unsigned short>() { return MPI_UNSIGNED_SHORT; }

/*
 * Template specialization for type `unsigned int`
 */
template <> inline decltype(auto) mpi_type<unsigned int>() { return MPI_UNSIGNED; }

/*
 * Template specialization for type `unsigned long`
 */
template <> inline decltype(auto) mpi_type<unsigned long>() { return MPI_UNSIGNED_LONG; }

/*
 * Template specialization for type `float`
 */
template <> inline decltype(auto) mpi_type<float>() { return MPI_FLOAT; }

/*
 * Template specialization for type `double`
 */
template <> inline decltype(auto) mpi_type<double>() { return MPI_DOUBLE; }

/*
 * Return MPI communicator size.
 */
inline int comm_size(MPI_Comm comm) {
    int i;
    MPI_Comm_size(comm, &i);
    return i;
}

/*
 * Return current MPI rank.
 */
inline int comm_rank(MPI_Comm comm) {
    int i;
    MPI_Comm_rank(comm, &i);
    return i;
}

/*
 * Call MPI_Sendrecv with correct data types and return result.
 */
template <typename T> T sendrecv(T sendval, int dest, int source, MPI_Comm comm) {
    T recvval{};
    MPI_Sendrecv(&sendval, 1, mpi_type<T>(), dest, 0, &recvval, 1, mpi_type<T>(), source, 0, comm, nullptr);
    return recvval;
}

/*
 * Call MPI_Allreduce with correct data types and return result.
 */
template <typename T> T allreduce(T sendval, MPI_Op op, MPI_Comm comm) {
    T recvval{};
    MPI_Allreduce(&sendval, &recvval, 1, mpi_type<T>(), op, comm);
    return recvval;
}

/*
 * Eigen namespace contains simple wrappers that work with Eigen arrays and automatically deduce the size of the
 * communication buffer. It also contains function for serialization of data.
 */
namespace Eigen {

template <int i, typename B, typename T> void _pack_buffer_entry(B buffer, T arg) {
    static_assert(B::RowsAtCompileTime == i + 1, "Your buffer has the wrong number of rows");
    buffer(i) = arg;
}

template <int i, typename B, typename T, typename... Ts> void _pack_buffer_entry(B buffer, T arg, Ts... args) {
    buffer(i) = arg;
    _pack_buffer_entry<i + 1>(buffer, args...);
}

/*
 * Serialization: Pack the arguments in order into the buffer
 */
template <typename B, typename... Ts> void pack_buffer_entry(B buffer, Ts... args) {
    _pack_buffer_entry<0>(buffer, args...);
}

template <int i, typename B, typename T> void _unpack_buffer_entry(B &buffer, T &arg) {
    static_assert(B::RowsAtCompileTime == i + 1, "Your buffer has the wrong number of rows");
    arg = buffer(i);
}

template <int i, typename B, typename T, typename... Ts> void _unpack_buffer_entry(B &buffer, T &arg, Ts &...args) {
    arg = buffer(i);
    _unpack_buffer_entry<i + 1>(buffer, args...);
}

/*
 * Serialization: Unpack the buffer into the arguments in order
 */
template <typename B, typename... Ts> void unpack_buffer_entry(B &buffer, Ts &...args) {
    _unpack_buffer_entry<0>(buffer, args...);
}

/*
 * Serialize specific entries of a number of arrays into a buffer. The mask specifies which entries are picked
 * from the arrays given in args.
 */
template <typename M, typename... Ts> decltype(auto) pack_buffer(M mask, const Ts &...args) {
    ::Eigen::Array<double, sizeof...(Ts), ::Eigen::Dynamic> buffer(sizeof...(Ts), mask.count());
    ::Eigen::Index buffer_index{0};
    for (::Eigen::Index i{0}; i < mask.size(); ++i) {
        if (mask[i]) {
            pack_buffer_entry(buffer.col(buffer_index), args(i)...);
            buffer_index++;
        }
    }
    assert(buffer_index == mask.count());
    return buffer;
}

/*
 * Deserialize specific entries of a number of arrays into a buffer. All buffer elements are inserted into the
 * arrays given in args starting with the position given by offset.
 */
template <typename B, typename... Ts> void unpack_buffer(B &buffer, ::Eigen::Index offset, const Ts &...args) {
    ::Eigen::Index buffer_index{0};
    for (auto &&buffer_entry : buffer.colwise()) {
        unpack_buffer_entry(buffer_entry, const_cast<Ts &>(args)(offset + buffer_index)...);
        buffer_index++;
    }
}

/*
 * Call MPI_Sendrecv for data stored in Eigen arrays; deduce data types and size automatically.
 */
template <typename SendType> decltype(auto) sendrecv(const SendType &sendarr, int dest, int source, MPI_Comm &comm) {
    // Negotiate buffer sizes.
    auto nb_recv{MPI::sendrecv(sendarr.cols(), dest, source, comm)};

    // Create receive buffer.
    using RecvType = ::Eigen::Array<typename SendType::Scalar, SendType::RowsAtCompileTime, ::Eigen::Dynamic>;
    RecvType recvarr(sendarr.rows(), nb_recv);

    // Some trickery: recvbuf for MPI_Sendrecv cannot be NULL, we hence use a dummy buffer when no data is
    // received.
    typename RecvType::Scalar dummy_recv_buffer[1];
    typename RecvType::Scalar *recv_buffer{recvarr.data()};
    if (!recv_buffer)
        recv_buffer = dummy_recv_buffer;

    MPI_Sendrecv(const_cast<SendType &>(sendarr).derived().data(), sendarr.size(),
                 mpi_type<typename SendType::Scalar>(), dest, 0, recv_buffer, recvarr.size(),
                 mpi_type<typename RecvType::Scalar>(), source, 0, comm, nullptr);

    return recvarr;
}

/*
 * Call MPI_Allreduce with correct data types.
 */
template <typename T> decltype(auto) allreduce(const T &sendarr, MPI_Comm comm) {
    using RecvType = ::Eigen::Array<typename T::Scalar, T::RowsAtCompileTime, T::ColsAtCompileTime>;
    RecvType recvarr(sendarr.rows(), sendarr.cols());
    MPI_Allgather(sendarr.data(), sendarr.size(), mpi_type<typename T::Scalar>(), recvarr.data(), recvarr.size(),
                  mpi_type<typename T::Scalar>(), comm);
    return recvarr;
}

/*
 * Call MPI_Allgather with correct data types.
 */
template <typename T, typename RecvType> void allgather(T &sendval, RecvType &recvbuf, MPI_Comm &comm) {
    MPI_Allgather(&sendval, 1, mpi_type<T>(), recvbuf.data(), 1, mpi_type<typename RecvType::Scalar>(), comm);
}

}  // namespace Eigen
}  // namespace MPI

#endif  // YAMD_MPI_SUPPORT_H
