#include <Eigen/Core>
#include <mpi.h>
#include <array>
#include <numeric>
#include <functional>
#include <chrono>
#include <iostream>

using namespace Eigen;

struct mpi_guard {
  mpi_guard() {
    MPI_Init(0, nullptr);
  }
  ~mpi_guard() {
    MPI_Finalize();
  }
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

bool is_left_rank(MPI_Comm comm) {
  int rank = 0;
  MPI_Comm_rank(comm, &rank);
  return rank == 0;
}

bool is_right_rank(MPI_Comm comm) {
  int rank = 0, size = 1;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);
  return rank == size - 1;
}

double second_order_fd(double left, double center, double right, double dx) {
  return (left - 2 * center + right) / (dx * dx);
}

ArrayXd second_order_fd(Ref<ArrayXd> left, Ref<ArrayXd> center, Ref<ArrayXd> right, double dx) {
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
void second_derivative_fd(MPI_Comm comm, Ref<ArrayXd> u_second, Ref<ArrayXd> u, double dx) {
  // Initiate communication
  Array2d ghosts, send;
  ghosts = 0;
  send(0) = u(0);
  send(1) = u(u.size() - 1);
  MPI_Neighbor_alltoall(send.data(), 1, MPI_DOUBLE,
                        ghosts.data(), 1, MPI_DOUBLE, comm);

  // MPI_Request request;
  // MPI_Ineighbor_alltoall(send.data(), 1, MPI_DOUBLE,
  //                        ghosts.data(), 1, MPI_DOUBLE, comm, &request);

  // Centered finite differences
  auto n = u.size() - 2;
  u_second.segment(1, n) = second_order_fd(u.head(n),
                                           u.segment(1, n),
                                           u.tail(n), dx);

  // Compute second derivative involving ghosts
  // MPI_Wait(&request, nullptr);
  n = u.size() - 1;
  u_second(0) = second_order_fd(ghosts(0), u(0), u(1), dx);
  u_second(n) = second_order_fd(u(n - 1), u(n), ghosts(1), dx);


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
void verlet(MPI_Comm comm, Ref<ArrayXd> u_second, Ref<ArrayXd> u_first, Ref<ArrayXd> u,
            double dt, double dx) {
  verlet_pred(u_second, u_first, u, dt);
  second_derivative_fd(comm, u_second, u, dx);
  verlet_corr(u_second, u_first, u, dt);
}

void synchronized_out(MPI_Comm comm, Ref<ArrayXd> u) {
  // Compute global size
  int N = 0, local_N = u.size();
  MPI_Reduce(&local_N, &N, 1, MPI_INT, MPI_SUM, 0, comm);

  auto rank = get_rank(comm), size = get_size(comm);
  ArrayXi sizes(size), displ(size);
  MPI_Gather(&local_N, 1, MPI_INT, sizes.data(), 1, MPI_INT, 0, comm);
  std::exclusive_scan(sizes.begin(), sizes.end(), displ.begin(), 0, std::plus<>());

  ArrayXd u_gathered((rank == 0) ? N : 1);
  MPI_Gatherv(u.data(), u.size(), MPI_DOUBLE,
              u_gathered.data(), sizes.data(), displ.data(), MPI_DOUBLE,
              0, comm);

  if (rank == 0)
    std::cout << u_gathered.transpose() << "\n";
}

int main() {
  mpi_guard guard{};  // Takes care of the calls to MPI_Init and MPI_Finalize

  // Problem definition
  auto N = 100000;
  auto L = 20.;
  auto dx = L / (N - 1);
  auto dt = 0.01;
  auto steps = 8000;

  // Get some MPI context
  auto size = get_size(MPI_COMM_WORLD);

  // Create a cartesian topology (a regular grid)
  MPI_Comm comm;
  std::array<int, 1> periodic{0};
  MPI_Cart_create(MPI_COMM_WORLD, 1, &size, periodic.data(), false, &comm);

  // Figure local size of problem
  auto rank = get_rank(comm);
  auto local_N = N / size;
  auto local_L = L / size;
  auto offset_N = rank * local_N;
  auto offset_L = rank * local_L;

  local_N += (rank == size - 1) ? N % size : 0;  // correct last rank

  // Initialize local x coordinates
  ArrayXd x(local_N);
  double start = -L / 2 + offset_L, end = start + local_L;

  if (is_right_rank(comm)) {
    x = ArrayXd::LinSpaced(local_N, start, end);
  } else { // ignore endpoint
    x = ArrayXd::LinSpaced(local_N + 1, start, end).head(local_N);
  }

  // Work variables
  ArrayXd u(local_N), u_first(local_N), u_second(local_N);

  // Initial conditions
  u = sin(4 * M_PI * x / L) * exp(-abs(x));
  u_first = 0;

  second_derivative_fd(comm, u_second, u, dx);

  using namespace std::literals;

  const auto tik = std::chrono::high_resolution_clock::now();

  for (auto i = 0; i < steps; ++i) {
    // synchronized_out(comm, u);
    verlet(comm, u_second, u_first, u, dt, dx);
  }

  const auto tok = std::chrono::high_resolution_clock::now();

  if (rank == 0) {
    std::cout << (tok - tik) / 1ms;
  }
}
