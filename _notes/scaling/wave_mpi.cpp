#include <Eigen/Core>
#include <array>
#include <chrono>
#include <iostream>
#include <mpi.h>
#include <sstream>

using namespace Eigen;

// RAII object for MPI init calls
struct mpi_guard {
  mpi_guard(int* argc, char*** argv) { MPI_Init(argc, argv); }
  mpi_guard() : mpi_guard(nullptr, nullptr) {}
  ~mpi_guard() { MPI_Finalize(); }
};

int get_rank(MPI_Comm comm) {
  int rank = 0;
  MPI_Comm_rank(comm, &rank);
  return rank;
}

int get_size(MPI_Comm comm) {
  int size = 0;
  MPI_Comm_size(comm, &size);
  return size;
}

bool is_left_rank(MPI_Comm comm) { return get_rank(comm) == 0; }

bool is_right_rank(MPI_Comm comm) {
  return get_rank(comm) == get_size(comm) - 1;
}

// Scalar overload of central finite difference formula
double second_order_fd(double left, double center, double right, double dx) {
  return (left - 2 * center + right) / (dx * dx);
}

// Array overload of central finite difference formula
ArrayXd second_order_fd(Ref<ArrayXd> left, Ref<ArrayXd> center,
                        Ref<ArrayXd> right, double dx) {
  return (left - 2 * center + right) / (dx * dx);
}

/// Apply boundary conditions
void apply_bc(MPI_Comm comm, Ref<ArrayXd> u_second, Ref<ArrayXd> u, double dx) {
  // Fixed boundary on left edge
  if (is_left_rank(comm))
    u_second(0) = 0;

  // Free boundary on right edge
  if (is_right_rank(comm)) {
    auto n = u.size() - 1;
    u_second(n) = second_order_fd(u(n - 1), u(n), u(n - 1), dx);
  }
}

/// Compute finite difference second derivative
void second_derivative_fd(MPI_Comm comm, Ref<ArrayXd> u_second, Ref<ArrayXd> u,
                          double dx) {
  // Communication setup:
  // ghosts contains u values from left and right procs
  // send contains u values to send to left and right procs
  Array2d ghosts, send;
  ghosts = 0;                 // initialize
  send(0) = u(0);             // send to left
  send(1) = u(u.size() - 1);  // send to right

  // Communicate data with neighbors
  MPI_Neighbor_alltoall(send.data(), 1, MPI_DOUBLE, ghosts.data(), 1,
                        MPI_DOUBLE, comm);

  // Nonblocking version of the communication (not necessarily faster)
  // The idea is to initiate the comm before computing our own data
  // so that when we are done the comm is finished

  // MPI_Request request;
  // MPI_Ineighbor_alltoall(send.data(), 1, MPI_DOUBLE,
  //                        ghosts.data(), 1, MPI_DOUBLE, comm, &request);

  // Centered finite differences on our own data
  auto n = u.size() - 2;
  u_second.segment(1, n) =
      second_order_fd(u.head(n), u.segment(1, n), u.tail(n), dx);

  // If nonblocking comms are used, we wait for ghost here
  // MPI_Wait(&request, MPI_STATUS_IGNORE);

  // Compute second derivative involving ghosts
  // For left and right ranks, some ghosts will be undefined
  // That's ok, we initialized things to zero and values will be
  // overwritten by the boundary conditions
  n = u.size() - 1;
  u_second(0) = second_order_fd(ghosts(0), u(0), u(1), dx);      // from left
  u_second(n) = second_order_fd(u(n - 1), u(n), ghosts(1), dx);  // from right

  // Finally apply boundary conditions
  apply_bc(comm, u_second, u, dx);
}

/// Verlet predictor step
void verlet_pred(Ref<ArrayXd> u_second, Ref<ArrayXd> u_first, Ref<ArrayXd> u,
                 double dt) {
  u += dt * u_first + (0.5 * dt * dt) * u_second;
  u_first += (dt * 0.5) * u_second;
}

/// Verlet corrector step
void verlet_corr(Ref<ArrayXd> u_second, Ref<ArrayXd> u_first, Ref<ArrayXd> u,
                 double dt) {
  u_first += (dt * 0.5) * u_second;
}

/// Verlet full step
void verlet(MPI_Comm comm, Ref<ArrayXd> u_second, Ref<ArrayXd> u_first,
            Ref<ArrayXd> u, double dt, double dx) {
  verlet_pred(u_second, u_first, u, dt);
  second_derivative_fd(comm, u_second, u, dx);
  verlet_corr(u_second, u_first, u, dt);
}

// Gather all data and print array values
// This is here for reference, but in a real world situation this
// type of output routine should be avoided and parallel IO preferred
void synchronized_out(MPI_Comm comm, int N, Ref<ArrayXd> u) {
  int local_N = u.size();

  // Gather local sizes and compute all offsets
  auto rank = get_rank(comm), size = get_size(comm);
  ArrayXi sizes(size), displ(size);
  sizes = displ = 0;
  MPI_Gather(&local_N, 1, MPI_INT, sizes.data(), 1, MPI_INT, 0, comm);

  for (int i = 1; i < size; ++i)
    displ[i] = sizes[i] + displ[i - 1];

  // Allocate receive buffer for rank 0
  ArrayXd u_gathered((rank == 0) ? N : 1);

  // We need "v" variant of gather since not all processes have the same
  // size of data (last one has more)
  MPI_Gatherv(u.data(), u.size(), MPI_DOUBLE, u_gathered.data(), sizes.data(),
              displ.data(), MPI_DOUBLE, 0, comm);

  // Print only on root rank
  if (rank == 0)
    std::cout << u_gathered.transpose() << "\n";
}

int main(int argc, char* argv[]) {
  mpi_guard guard;  // Takes care of the calls to MPI_Init and MPI_Finalize

  // Load arguments
  std::stringstream args;
  for (int i = 1; i < argc; ++i)
    args << argv[i];

  // Problem definition
  int N = 1000;

  // Read from argument if present
  if (argc >= 2)
    args >> N;

  auto L = 20.;
  auto dx = L / (N - 1);
  auto dt = 0.01;
  auto steps = 8000;

  // Get some MPI context
  auto size = get_size(MPI_COMM_WORLD);

  // Create a cartesian topology (a regular grid in 1d)
  MPI_Comm comm;
  std::array<int, 1> periodic{0};
  MPI_Cart_create(MPI_COMM_WORLD, 1, &size, periodic.data(), false, &comm);

  // Figure local size of problem
  auto rank = get_rank(comm);
  auto local_N = N / size;
  auto local_L = L / size;
  auto offset_L = rank * local_L;

  local_N += (rank == size - 1) ? N % size : 0;  // correct last rank

  // Initialize local x coordinates
  ArrayXd x(local_N);
  double start = -L / 2 + offset_L, end = start + local_L;

  if (is_right_rank(comm)) {
    x = ArrayXd::LinSpaced(local_N, start, end);
  } else {  // ignore endpoint
    x = ArrayXd::LinSpaced(local_N + 1, start, end).head(local_N);
  }

  // Work variables are local
  ArrayXd u(local_N), u_first(local_N), u_second(local_N);

  // Initial conditions
  u = sin(4 * M_PI * x / L) * exp(-abs(x));
  u_first = 0;

  second_derivative_fd(comm, u_second, u, dx);

  using namespace std::literals;
  using clock = std::chrono::high_resolution_clock;

  // We time our run loop
  const auto tik = clock::now();
  for (auto i = 0; i < steps; ++i) {
    synchronized_out(comm, N, u);
    verlet(comm, u_second, u_first, u, dt, dx);
  }
  const auto tok = clock::now();

  // Print loop time on root rank (in miliseconds)
  // if (rank == 0) {
  //   std::cout << (tok - tik) / 1ms;
  // }

  return 0;
}
