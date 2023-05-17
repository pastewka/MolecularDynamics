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
BENCHMARK_MAIN();
