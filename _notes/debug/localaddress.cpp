double* unit_3d_vector() {
  double v[3] = {0, 0, 1};
  return v;
}

int main() {
  double* n = unit_3d_vector();
  for (int i = 0; i < 3; ++i)
    n[i] += 1;
  delete[] n;
  return 0;
}
