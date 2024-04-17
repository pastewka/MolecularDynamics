---
layout: default
title:  "Milestone 04"
parent: Project
categories: project
author : Lars Pastewka
nav_order: 5
---

# Milestone 4 <br/> Lennard-Jones potential with direct summation

## Learning goals

The student will...
* ...learn how to implement a simple interatomic potential.
* ...learn how to systematically test the implementation of this interatomic potential.

## Introduction

We will now implement our first interatomic potential: A Lennard-Jones interaction. The
energy of a system interacting via Lennard-Jones forces is given by

$$E_\text{pot} = \frac{1}{2} \sum_{ij} 4 \varepsilon \left[ \left(\frac{\sigma}{r_{ij}}\right)^{12} - \left(\frac{\sigma}{r_{ij}}\right)^6\right]$$

The term \\(\propto r^{-12}\\) is a simple model for Pauli repulsion and the term \\(\propto r^{-6}\\) is a model for London dispersion forces.
Within this milestone, you will implement this potential via direct summation, i.e. directly using the equation given above without cutting it off at a certain distance.

Note that it can be useful to create a separate build target with a separate `main.cpp` for this milestone. Our notes on [`Meson`](../_notes/Meson) describe how to do that.

## A data structure for the atomic system

Before starting, we introduce a data structure that holds the information on the atomic system, i.e. the positions, velocities, forces etc.
This makes it easier to pass the atomic system around. We suggest a data structure of the form:
```c++
using Positions_t = Eigen::Array3Xd; 
using Velocities_t = Eigen::Array3Xd; 
using Forces_t = Eigen::Array3Xd; 
 
class Atoms { 
public: 
    Positions_t positions; 
    Velocities_t velocities; 
    Forces_t forces; 
 
    Atoms(const Positions_t &p) : 
            positions{p}, velocities{3, p.cols()}, forces{3, p.cols()} { 
        velocities.setZero(); 
        forces.setZero(); 
    } 
 
    Atoms(const Positions_t &p, const Velocities_t &v) : 
            positions{p}, velocities{v}, forces{3, p.cols()} { 
        assert(p.cols() == v.cols());
        forces.setZero(); 
    } 

    size_t nb_atoms() const { 
        return positions.cols(); 
    }
};
```
The [`const` qualifier](https://en.cppreference.com/w/c/language/const) behind `nb_atoms` tells the compiler that this method does not change the state of the `Atoms` object, i.e. the value of `positions`, `velocities` and `forces` are not affected by a call to `nb_atoms`.
Place this data structure in a separate header file, e.g. `atoms.h`. We had already discussed in [Milestone 3] that the types should reside in their own
header, e.g. `types.h`. You can then simply include them in `atoms.h` by placing `#include "types.h"` somewhere at the beginning of the file. Make sure all header files have [header guards](https://en.wikipedia.org/wiki/Include_guard).

## Signature of the function that computes the interatomic potential

Implement the Lennard-Jones potential. Note that you will need to derive the _analytical_ gradient of the total energy

$$\vec{f}_k = -\nabla_k E$$

before you can implement the analytical forces. The final expression should take the form

$$\vec{f}_k = \sum_i \vec{f}_{ik}$$

where \\(\vec{f}\_{ik}\\) is a pair force. (You need to evaluate the specific expression for \\(\vec{f}\_{ik}\\) yourself.) This type of expression is most efficiently computed by looping over unique pairs.

We suggest a function with the following signature:
```c++
double lj_direct_summation(Atoms &atoms, double epsilon = 1.0, double sigma = 1.0);
```
This signature defines default parameters for `epsilon` and `sigma`, i.e. they need to be specified only if they differ from unity. The function directly modifies the forces in `atoms.forces`. The return value of this function is the potential energy. Place this function in its own source and
header file, e.g. `lj_direction_summation.h` and `lj_direct_summation.cpp`. 

## Testing the implementation

When implementing this function you will want to test that the forces that are computed are correct. In particular, the forces and energy need to be
consistent in the sense that the forces are the negative derivative of the energy. The energy is typically easy to implement correctly, but forces
can be fiddly. One sign of wrong forces is that energy in a molecular dynamics simulation is not conserved, but there are also other causes for
this effect (e.g. a time step that is too large) which makes it difficult as a test for the implementation of forces.

The common strategy is to compute the forces _numerically_ from the energies. For this, we have to compute a numerical first derivative.
This is straightforward to do from the difference quotient, e.g.

$$ f'(x) \approx \frac{f(x+\Delta x) - f(x-\Delta x)}{2 \Delta x}$$

A formal derivation of this expression can be obtained from a Taylor expansion of \\(f(x)\\). We can use this expression to compute numerical
estimates of the forces. A test that uses this to test the analytical forces of the potential can be found here: [test_lj_direct_summation.cpp](test_lj_direct_summation.cpp).
Place this file in your `tests` subdirectory and add it to the `meson.build` to use it. Open it in an editor and try to understand how the test works.
This type of test is often called a _gradient test_.

## A first molecular dynamics calculation

You are now in a position to run a first molecular dynamics calculation. To do this, you need a reasonable initial state for your
simulation. The initial state is the initial condition for the solution of Newton's equation of motion and requires you to
specify positions and momenta. You can find such an initial state in the following file: [lj54.xyz](lj54.xyz). The format of this file is called the [`XYZ` file format](https://en.wikipedia.org/wiki/XYZ_file_format). We provide a C++ module for reading and writing these files here: [xyz.h](xyz.h), [xyz.cpp](xyz.cpp). Include these files into your project. Also open them in an editor and try to understand what they do.

You can then read an XYZ-file using the following code block:
```c++
#include "xyz.h"
auto [names, positions, velocities]{read_xyz_with_velocities("lj54.xyz")};
```
The variable `names` contains the element names that you can discard at this point. Important are the variables `positions` and `velocities` that you should use as the initial state of your simulation.
Note that the velocities contained in [lj54.xyz](lj54.xyz) are an extension to the typical [`XYZ` file format](https://en.wikipedia.org/wiki/XYZ_file_format) that is, however, understood by common visualization tools.

Update your code to read this file and then propagate the simulation for a total time of at least \\(100 \sqrt{m\sigma^2/\varepsilon} \\). Use a mass of unity (\\(m=1)\\) and \\(\varepsilon=1\\) and \\(\sigma=1\\) for the Lennard-Jones interaction. A reasonable initial time step is \\( 0.001 \sqrt{m\sigma^2/\varepsilon} \\).
Monitor the total energy of your simulation. For this you need to implement the computation of the kinetic energy.

At this point you can quantify the influence of the time step on your simulation. Change the time step and see how the total energy evolves. What is a good time step for your simulation?

## Visualization

Since you have now run the first molecular dynamics calculation, it is useful to visualize your simulation, i.e. look at how
the individual atoms move over time. To achieve this, output the state of the simulation as an `XYZ` at time intervals of order \\(1 \sqrt{m\sigma^2/\varepsilon} \\).

`XYZ`-files can be visualized with the [Open Visualization Tool (OVITO)](https://www.ovito.org/). Download _OVITO Basic_, install it and look at one of your `XYZ` files. To output trajectories, you can do two things:
* Consecutively number your files, e.g. `traj0000.xyz`, `traj0001.xyz`, `traj0002.xyz`, etc., OVITO will automatically detect that this is a sequence of files (a _trajectory_) and allow you visualize the time evolution of your atomic configuration.
* Write multiple `XYZ`s into the same file. Also here OVITO will autodetect that this is a trajectory. The following code snippets where `write_xyz` can be repeated as often as necessary does this:

```c++
std::ofstream traj("traj.xyz");
...
write_xyz(traj, atoms);
...
write_xyz(traj, atoms);
...
traj.close();
```

The second approach has the advantage that there is only a single file that you need to work with, but it can be difficult to extract individual frames from it (e.g. for restarts of your simulation).

## Task summary

This milestone requires the following tasks:

* Derive the analytical expression for the forces of the Lennard-Jones potential
* Implement the Lennard-Jones potential and make sure the gradient test passes
* Implement computation of the kinetic energy
* Run a first molecular dynamics simulation
* Decide on a "good" time step
* Download and install OVITO
* Visualize your simulation

We ask you to provide the following analytical results in your final report:

* Derivation of the analytical expression for the forces of the Lennard-Jones potential

We ask you to provide and discuss the following figures in your final report:

* Plot of the total energy as a function of time for different time steps
* A sequence of snapshots (no more than 5) from your simulation run

We provide the following files for you:

* [lj54.xyz](lj54.xyz)
* [test_lj_direct_summation.cpp](test_lj_direct_summation.cpp)
* [xyz.h](xyz.h)
* [xyz.cpp](xyz.cpp)
