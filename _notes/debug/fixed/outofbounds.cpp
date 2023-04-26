#include <cstddef>
#include <span>
#include <vector>
#include <Eigen/Dense>

double square_and_sum(std::span<double> v) {
  double res = 0;
  for (auto& value : v) {
    value *= value;
    res += value;
  }
  return res;
}

double square_and_sum(Eigen::Ref<Eigen::ArrayXd> v) {
  v *= v;
  return v.sum();
}

int main() {
  std::size_t n = 5;
  std::vector<double> v1(n);
  square_and_sum(v1);

  Eigen::VectorXd v2(n);
  // VectorXd is automatically converted to ArrayXd here
  square_and_sum(v2);
  return 0;
}

