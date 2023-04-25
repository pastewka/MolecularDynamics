double square_and_sum(double * v, int n) {
  double res = 0;
  for (int i = 1; i <= n; ++i) {
    v[i] *= v[i];
    res += v[i];
  }
  return res;
}

int main() {
  int n = 20;
  double * v = new double[n];
  square_and_sum(v, n);
  delete[] v;
  return 0;
}

