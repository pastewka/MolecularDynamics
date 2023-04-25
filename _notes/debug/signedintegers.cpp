#include <vector>
#include <iostream>

void assign(std::vector<std::pair<double, double>>& v, double dx) {
  std::size_t n = v.size();
  for (std::size_t k = 0; k < n; k++) {
    auto& [a, b] = v[k]; // structured binding to get references to pair values
    a = dx * (n - k);
    b = dx * (k - n);
  }
}

int main() {
  std::vector<std::pair<double, double>> v(3);
  double increment = 0.2;
  assign(v, increment);
  // Structured binding (C++17) pair -> 2 variables
  for (auto [a, b] : v)
    std::cout << "a = " << a << ", b = " << b << '\n';
  return 0;
}
