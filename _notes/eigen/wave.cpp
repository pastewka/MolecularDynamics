#include <Eigen/Core>
#include <iostream>

using namespace Eigen;

/// Apply boundary conditions
void apply_bc(Ref<ArrayXd> u_second, Ref<ArrayXd> u, double dx) {
  // Fixed boundary on left edge
  u_second(0) = 0;

  // Free boundary on right edge
  auto n = u.size() - 1;
  u_second(n) = (u(n - 1) - 2 * u(n) + u(n - 1)) / (dx * dx);
}

/// Compute finite difference second derivative
void second_derivative_fd(Ref<ArrayXd> u_second, Ref<ArrayXd> u, double dx) {
  // Select everything except edges
  auto n = u.size() - 2;

  // Centered finite differences
  u_second.segment(1, n) =
      (u.head(n) - 2 * u.segment(1, n) + u.tail(n)) / (dx * dx);

  apply_bc(u_second, u, dx);
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
void verlet(Ref<ArrayXd> u_second, Ref<ArrayXd> u_first, Ref<ArrayXd> u,
            double dt, double dx) {
  verlet_pred(u_second, u_first, u, dt);
  second_derivative_fd(u_second, u, dx);
  verlet_corr(u_second, u_first, u, dt);
}

int main() {
  auto N = 1000;
  auto L = 20.;
  auto dx = L / (N - 1);
  auto dt = 0.01;
  auto steps = 8000;
  auto x = ArrayXd::LinSpaced(N, -L / 2, L / 2);

  ArrayXd u(N), u_first(N), u_second(N);

  u = sin(4 * M_PI * x / L) * exp(-abs(x));

  second_derivative_fd(u_second, u, dx);

  for (auto i = 0; i < steps; ++i) {
    std::cout << u.transpose() << "\n";
    verlet(u_second, u_first, u, dt, dx);
  }
}
