#include <array>
#include <Eigen/Dense>

Eigen::Vector3d unit_3d_eigen_vector() {
  Eigen::Vector3d v{{0, 0, 1}};
  return v;
}

std::array<double, 3> unit_3d_vector() {
  std::array<double, 3> v{{0, 0, 1}};
  return v;
}

int main() {
  auto n = unit_3d_vector();
  for (auto& value : n)
    value += 1;

  auto eigen_n = unit_3d_eigen_vector();

  // For element-wise operations we need an Array3d, not a Vector3d
  eigen_n.array() += 1;
  return 0;
}
