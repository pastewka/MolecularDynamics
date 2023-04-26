#include <cstddef>
#include <span>
#include <vector>
#include <Eigen/Core>

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

#if 0
// Version of the function with bounds checking in "Debug" mode
double square_and_sum(Eigen::Ref<Eigen::ArrayXd> v) {
  double res = 0;
  // This will access out-of-bounds in "Release" mode!
  for (int i = 1; i <= v.size(); ++i) {
    v(i) *= v(i);
    res += v(i);
  }
  return res;
}
#endif

int main() {
  std::size_t n = 5;
  std::vector<double> v1(n);
  square_and_sum(v1);

  Eigen::VectorXd v2(n);
  // VectorXd is automatically converted to ArrayXd here
  square_and_sum(v2);
  return 0;
}

