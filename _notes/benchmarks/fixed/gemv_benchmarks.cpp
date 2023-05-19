#include <Eigen/Core>
#include <benchmark/benchmark.h>
#include <vector>

#define MY_BENCHMARK(function)                                                 \
  BENCHMARK(function)->Range(8, 8 << 11)->Complexity(benchmark::oNSquared)

using namespace Eigen;

static void BM_GEMV_vectors_nested(benchmark::State& state) {
  std::size_t N = state.range(0);
  std::vector<std::vector<double>> A(N);
  std::vector<double> v(N), result(N);

  for (auto& row : A) {
    row.resize(N);
  }

  for (auto _ : state) {
    for (std::size_t i = 0; i < N; ++i) {
      for (std::size_t j = 0; j < N; ++j) {
        result[i] += A[i][j] * v[j];
      }
    }
  }

  state.SetComplexityN(N);
}

MY_BENCHMARK(BM_GEMV_vectors_nested);

/* ----------------------------------------------------------------------- */
static void BM_GEMV_vectors_rowmajor(benchmark::State& state) {
  std::size_t N = state.range(0);
  std::vector<double> A(N * N), v(N), result(N);

  for (auto _ : state) {
    // Loop order is important
    for (std::size_t i = 0; i < N; ++i) {
      // Fastest index should be contiguous access
      for (std::size_t j = 0; j < N; ++j) {
        result[i] += A[i * N + j] * v[j];
      }
    }
  }

  state.SetComplexityN(N);
}

MY_BENCHMARK(BM_GEMV_vectors_rowmajor);

/* ----------------------------------------------------------------------- */
static void BM_GEMV_vectors_colmajor(benchmark::State& state) {
  std::size_t N = state.range(0);
  std::vector<double> A(N * N), v(N), result(N);

  for (auto _ : state) {
    // Loop order is important
    for (std::size_t j = 0; j < N; ++j) {
      // Fastest index should be contiguous access
      for (std::size_t i = 0; i < N; ++i) {
        result[i] += A[i + N * j] * v[j];
      }
    }
  }

  state.SetComplexityN(N);
}

MY_BENCHMARK(BM_GEMV_vectors_colmajor);

/* ----------------------------------------------------------------------- */
static void BM_GEMV_eigen_loops(benchmark::State& state) {
  std::size_t N = state.range(0);
  MatrixXd A(
      N, N); // by default ColMajor, cf
             // https://eigen.tuxfamily.org/dox/group__TopicStorageOrders.html
  VectorXd v(N), result(N);

  for (auto _ : state) {
    // ColMajor loop ordering
    for (std::size_t j = 0; j < N; ++j) {
      for (std::size_t i = 0; i < N; ++i) {
        result(i) += A(i, j) * v(j);
      }
    }
  }

  state.SetComplexityN(N);
}

MY_BENCHMARK(BM_GEMV_eigen_loops);

/* ----------------------------------------------------------------------- */
static void BM_GEMV_vectors_colmajor_aligned(benchmark::State& state) {
  std::size_t N = state.range(0);
  std::vector<double, aligned_allocator<double>> A(N * N), v(N), result(N);

  for (auto _ : state) {
    // Loop order is important
    for (std::size_t j = 0; j < N; ++j) {
      // Fastest index should be contiguous access
      for (std::size_t i = 0; i < N; ++i) {
        result[i] += A[i + N * j] * v[j];
      }
    }
  }

  state.SetComplexityN(N);
}

MY_BENCHMARK(BM_GEMV_vectors_colmajor_aligned);

/* ----------------------------------------------------------------------- */
static void BM_GEMV_eigen(benchmark::State& state) {
  std::size_t N = state.range(0);
  MatrixXd A(N, N);
  VectorXd v(N), result(N);

  for (auto _ : state) {
    result = A * v;
  }

  state.SetComplexityN(N);
}

MY_BENCHMARK(BM_GEMV_eigen);

/* ----------------------------------------------------------------------- */
static void BM_GEMV_eigen_fixed_size_inline(benchmark::State& state) {
  Matrix<double, 4, 4> A;
  Vector<double, 4> result, v;

  for (auto _ : state) {
    auto data = result.data();      // creating named variable important here
    benchmark::DoNotOptimize(data); // tell the compiler that data is important
                                    // this prevents optimization of the gemv

    // the generated assembly code for the matrix-vector product should be:
    // - vectorized: mulpd and addpd instructions for computation, movupd/movapd
    // - unrolled: no jumps or comparisons, no loop counter
    // - inlined: no call instruction to an external function
    result = A * v;
  }

  // see
  // https://github.com/google/benchmark/blob/main/docs/user_guide.md#preventing-optimization
  // for more info on preventing optimizations (do not use outside of
  // benchmarks!)
}

BENCHMARK(BM_GEMV_eigen_fixed_size_inline);

/* ----------------------------------------------------------------------- */
BENCHMARK_MAIN();
