double* unit_vector() {
  double v[3] = {0, 0, 1};
  return v;
}

int main() {
  double* n = unit_vector();
  for (int i = 0; i < 3; ++i)
    n[i] += 1;
  return 0;
}
