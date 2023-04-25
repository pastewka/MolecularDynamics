double square_and_sum(double * v, int n) {
  double res = 0;
  for (int i = 1; i <= n; ++i) {
    v[i] *= v[i];
    res += v[i];
  }
  return res;
}

int main() {
  std::size_t n = 5;
  double * v1 = new double[n];
  square_and_sum(v1, n);
  delete[] v1;
  return 0;
}

