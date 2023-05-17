#include <Eigen/Dense>
#include <iostream>

using namespace Eigen;

double kinetic_energy(Ref<Array3Xd> velocities, Ref<ArrayXd> masses) {
  ArrayXd squared_velocities = velocities.colwise().squaredNorm();
  ArrayXd ek_per_particle = 0.5 * masses * squared_velocities;
  return ek_per_particle.sum();
}

int main() {
  std::size_t N = 3;
  Array3Xd velocities(3, N);
  ArrayXd masses(N, 1);

  velocities << 1, 1, 1, 0, 1, 0, 1, 0, 1;
  masses << 1, 2, 1;

  std::cout << "v_i = \n" << velocities << '\n'
            << "m_i = \n" << masses.transpose() << '\n';

  double Ke = kinetic_energy(velocities, masses);

  std::cout << Ke << '\n';
  return 0;
}
