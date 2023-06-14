---
layout: default
title:  "Milestone 06"
parent: Project
categories: project
author : Lars Pastewka
nav_order: 7
---

# Milestone 6 <br/> Lennard-Jones potential with a cutoff

## Learning goals

The student will...
* ...learn how to work with neighbor lists.

## Introduction

In this milestone, we will start working with neighbor lists that are useful for computing short ranged potential. These types of
data structures are sometimes also called [Verlet lists](https://en.wikipedia.org/wiki/Verlet_list). We will provide an
implementation of the neighbor search for you, but you are also welcome to implement your own neighbor search. Should you
decide on your own implementation, please point this out to us in your written report and add a brief description of the
algorithm.

## Neighbor list search

The rough alorigthm for searching for neighbors is described in the lecture
material. We here provide an implementation and tests for you:
[neighbors.h](neighbors.h) [neighbors.cpp](neighbors.cpp)
[test_neighbors.cpp](test_neighbors.cpp). The neighbor list class can be
instanciated as:
```c++
NeighborList neighbor_list;
```
The object `neighbor_list` (or whatever variable name you may have chosen) then
holds the neighbor list. Update the neighbor list with a cutoff value:
```c++
neighbor_list.update(atoms, 5.0);
```
where `atoms` is an object of type `Atoms` that holds you positions and velocities. You can then loop over _pairs_ of atoms
```c++
for (auto[i, j]: neighbor_list) {
    do_something_fancy();
}
```
where `i` and `j` hold the indices of the atoms belonging to the pair. Note that this is a neighbor list (not a pair list), so each pair will occur twice. If you need to loop over pairs, use
```c++
for (auto[i, j]: neighbor_list) {
    if (i < j) {
        do_something_fancy();
    }
}
```
to exclude one occurence of the pair explicitly.

## Lennard-Jones potential with a cutoff

Copy your Lennard-Jones implementation (to a file `lj.h` and `lj.cpp` or similar) and modify it to use the neighbor list. Note
that you need a new parameter, the cutoff distance. Make sure what you shift the energy to zero at the cutoff distance as discussed in the lecture material.

## Computational complexity

Rerun the equilibration steps from Milestone 5 and measure the execution time as a function of the number of atoms in the system. Compare these results to those of Milestone 5. Discuss what you see and why.

## Task summary

This milestone requires the following tasks:

* Modify your Lennard-Jones implementation to use neighbor lists
* Measure the simulation time as a function of the number of atoms in your system

Optional:

* Implement your own neighbor list search

We ask you to provide and discuss the following figures in your final report:

* Plot of the simulation time as a function of the size (number of atoms) of your atomic system

We provide the following files for you:

* [neighbors.h](neighbors.h)
* [neighbors.cpp](neighbors.cpp)
* [test_neighbors.cpp](test_neighbors.cpp)
