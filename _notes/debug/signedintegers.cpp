#include <vector>
#include <iostream>

void assign(std::vector<double>& v, double dx) {
  std::size_t n = v.size();
  for (std::size_t k = 0; k < n; k++)
    v[k] = dx * (k - n);
}

int main() {
  std::vector<double> v(3);
  double increment = 0.2;
  assign(v, increment);

  for (auto&& val : v)
    std::cout << val << " ";
  std::cout << "\n";
  return 0;
}
