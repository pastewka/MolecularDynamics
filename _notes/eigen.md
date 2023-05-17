---
layout: default
title:  "Eigen"
parent: Notes
categories: notes
author : Lucas Frérot
nav_order: 3
---

Here are the helper files for this section:

- [CMakeLists.txt](eigen/CMakeLists.txt)
- [wave.cpp](eigen/wave.cpp)
- [kinetic_energy.cpp](eigen/kinetic_energy.cpp)
- [animate.py](eigen/animate.py)
- [plot.py](eigen/plot.py)

# Kinetic energy

We want to compute, with Eigen arrays:

$$ E_k = \sum_{i=1}^N{\frac{1}{2} m_i v_i^2},$$

where $v_i^2 = \vec{v_i}\cdot\vec{v_i} = \|\vec{v_i}\|^2$. Let's start with an
empty function taking as inputs a velocity array and masses array:

```cpp
// Ref<EigenType> is how you pass a reference to an Eigen array/vector/matrix
double kinetic_energy(Ref<Array3Xd> velocities, Ref<ArrayXd> masses) {
  return 0;
}
```

You can observe that the type for `velocities` is particular: it has a fixed
size of 3 rows times X columns (X being the number of atoms). Each column in
`velocities` is therefore an atom's velocity vector. Since masses could be
different for different atoms, we need to compute $v_i$ in a *column wise*
fashion. Let's define a `squared_velocity` array:

```cpp
ArrayXd squared_velocities = velocities.colwise().squaredNorm();
```

That lets us define the energy per particle:

```cpp
ArrayXd ek_per_particle = 0.5 * masses * squared_velocities;
```

And finally we can obtain the total kinetic energy with `ek_per_particle.sum()`.

# Scalar wave propagation with Eigen

A very simple way to solve the scalar wave equation:

$$ \frac{\partial^2 u}{\partial x^2} = \frac{\partial^2 u}{\partial t^2}, $$

is to use a centered finite difference approximation for the space derivative
(with $x$) and time derivative (with $t$). The latter leads to the velocity
Verlet algorithm seen in class. The former follows the formula:

$$ \frac{\partial^2 u}{\partial x^2}(x_i) \approx \frac{u(x_{i-1}) - 2 u(x_i)
      + u(x_{i+1})}{\Delta x^2}, $$
      
where $\Delta x$ is the spacing between $x_i$ and $x_{i+1}$. We now need
boundary conditions to the wave equation. Here we select a fixed end $x = -L/2$ and free
end at $x = L/2$, which translate to:

$$ \partial^2_x u(x_0) = 0,\ u(x_{N+1}) = u(x_{N-1}) $$

This scheme, with the Verlet integration, can be entirely written without loops
using Eigen operations like `head`, `segment` and `tail`, which we can see in
[wave.cpp](eigen/wave.cpp).

You can plot (with [Numpy](https://numpy.org/) and
[Matplotlib](https://matplotlib.org/)) the result of the simulation by piping
the standard output to a plot script:

```bash
./wave | ../animate.py
```

Try benchmarking the `wave` program with and without the `std::cout` call in the
time loop.
