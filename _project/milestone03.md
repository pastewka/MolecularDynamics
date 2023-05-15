---
layout: default
title:  "Milestone 03"
parent: Project
categories: project
author : Lars Pastewka
nav_order: 4
---

# Milestone 3 <br/> Velocity-Verlet integration for multiple atoms

## Learning goals

The student will...
* ...learn how to work with [Eigen arrays].

## Introduction

The implementation of the integration algorithm from Milestone 2 only works for a single atom.
You will now extend the function `verlet_step1` and `verlet_step2` to work with multiple atoms.
For this we will have to introduce data structure that can store the positions and velocities
of multiple atoms. We will use [Eigen arrays] for this.

## Basic data structures

Arrays store multiple values of the same data type and can be used to represent, for example, positions
of all atoms. Arrays are indexed and can be though of as the realization of the mathematical
objects that are vectors, matrices or tensors. We would like to emphasize, that choosing the right
data structures is the single most important part for obtaining code that performs well.

As a general rule, the data structure should be designed in a way that data that is processed consecutively is also stored in memory in a continuous manner.
This ensures [cache coherence](https://en.wikipedia.org/wiki/Cache_coherence).
For example, we could be tempted to create a class `Atom` that contains the positions, velocities, etc. of a single atom and than use an array (e.g. `std::vector<Atom> atoms`) of that class as the basic data structure. However, positions are then no longer consecutive in memory, since the `Atom` class contains velocities, possible forces, maybe charges and other properties. A function (e.g. computing forces) does not need the velocities would still load them into the cache, as the cache line size for all modern CPUs is  bytes. For high-performance numerical code, it is therefore always preferable to use structures of arrays rather than arrays of structure.

In order to store the positions, we will use an Eigen array of shape \\(3\times N\\) where \\(N\\) is the total number of atoms in our system. You can define an alias to that type with the [`using`](https://en.cppreference.com/w/cpp/language/type_alias) keyword:
```c++
using Positions_t = Eigen::Array3Xd;
``` 
We recommend defining similar aliases for the velocities, forces etc., even if they point to the same type. This enhances readability. Place those type aliases in a separate header file, e.g. `types.h`. [`Eigen::Array3Xd`](https://eigen.tuxfamily.org/dox/group__arraytypedefs.html) is a pre-defined array
type that stores `double`s (hence the suffix `d`), has \\(3\\) rows (this is fixed at compile time) and a dynamic number of
columns (indicated by the `X`). [Eigen arrays] are stored [column major](https://en.wikipedia.org/wiki/Row-_and_column-major_order),
hence the \\(3\\) row entries are stored consecutive in memory. Note that Eigen only supports \\(1\\)- and \\(2\\)-dimensional arrays.

## Working with Eigen arrays

We will here outline a couple of important features of [Eigen arrays]. First, when initializing an array you
need to specify the array size:
```c++
int nb_atoms = 10;
Positions_t positions(3, nb_atoms);
```
Note that the number of rows (the first argument) is fixed but we still need to provide it upon initialization.
The second argument `nb_atoms` is variable. This combination of fixed-variable sizes is indicated by the `3X`
suffix of the array type used here to store the positions.

You can access values of the array using parenthesis, e.g.
```c++
positions(2, 1) = 1.0;
```
sets the \\(y\\)-component of the position of the second atom to \\(1\\). (Important: Indices start at \\(0\\)!)
If you want to access the \\(3\\)-vector containing all positions of the second atom, you can use
```c++
auto pos2{positions.col(1)};
```
This yields _column_ with index \\(1\\), but each column stores the three Cartesian components of the position
in our case. You can for example compute the distance vector between two atoms \\(i\\) and \\(j\\) using
```c++
Eigen::Vector3d distance_vector{positions.col(i) - positions.col(j)};
```
Note that in the above example we have use the [`auto`](https://en.cppreference.com/w/cpp/language/auto) keyword
to automatically derive the type, while in the second case we have explicitly used the data type [`Eigen::Vector3d`](https://eigen.tuxfamily.org/dox/group__TutorialMatrixClass.html)
for the distance vector. The curly brackets are a [non-narrowing initialization](https://en.cppreference.com/w/cpp/language/direct_initialization). This type of initialization avoids implicit type conversion and should be prefered over
copy initialization or narrowing initialization (with round parenthesis).

## Integrator for multiple atoms

You are now in a position to turn your integrator into one that accepts multiple atoms.
We suggest to use the following signature for the integrators:
```c++
#ifndef VERLET_H
#define VERLET_H

#include <Eigen/Dense>

void verlet_step1(Eigen::Array3Xd &positions, Eigen::Array3Xd &velocities,
                  const Eigen::Array3Xd &forces, double timestep);
void verlet_step2(Eigen::Array3Xd &velocities, const Eigen::Array3Xd &forces, double timestep);

#endif  // VERLET_H
```
The [`const` qualifier](https://en.cppreference.com/w/c/language/const) tells the compiler that a modification
of that argument is not allowed, although it is passed as a reference.
(We don't need to modify the forces and it is good practice to protect the with this `const` qualifier
to avoid accidental modification.)

The corresponding source file `verlet.cpp` should look like this:
```c++
#include "verlet.h"

void verlet_step1(Eigen::Array3Xd &positions, Eigen::Array3Xd &velocities,
                  const Eigen::Array3Xd &forces, double timestep) {
    ... implement Verlet step1 here ...
}

void verlet_step2(Eigen::Array3Xd &velocities, const Eigen::Array3Xd &forces, double timestep) {
    ... implement Verlet step2 here ...
}
```
Note that you only need to make minor modifications to your current, single-atom integrator, as the loop over
atoms occurs automatically. For example, you can obtain an array containing all \\(x\\)-positions from `positions.row(0)`.

## Testing the integrator

Update the tests such that they test your modified integrator. These tests should probably propagate a few atoms at the
same time.

[Eigen arrays]: https://eigen.tuxfamily.org/dox/group__TutorialArrayClass.html
[Eigen matrices and vectors]: https://eigen.tuxfamily.org/dox/group__TutorialMatrixClass.html

## Task summary

This milestone requires the following tasks:

* Extend your Velocity-Verlet integrator to work with multiple atoms
* Update your tests to accomodate the changed interface
