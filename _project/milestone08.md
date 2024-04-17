---
layout: default
title:  "Milestone 08"
parent: Project
categories: project
author : Lars Pastewka
nav_order: 9
---

# Milestone 8 <br/> Parallelization

## Learning goals

The student will...
* ...learn how to work with the message passing interface (MPI).
* ...be able to use domain decomposition as a parallelization strategy.
* ...learn how to run parallel simulations on supercomputers.

## Introduction

The system sizes that are accessible with our code are limited by the computational power of a single CPU. Parallelization allows us to use multiple CPUs for the computing task at hand. In this milestone, we will parallelize the code using a strategy called *domain decomposition*. The physical domain of our simulation is decomposed in to subdomains that each "live" on a single CPU. More specifically, we will use a Cartesian decomposition of the overall domain. Since both potential encounterd so far (Lennard-Jones and EAM) are short-ranged, meaning the interaction only extends to a certain distance, the necessary subdomain need to "talk" only to their respective neighbors. This leads to a communication pattern that effectively scales extremely well, easily beyond $$100\,000$$ CPUs.

## MPI prerequisites

Starting with this milestone, we will make use of the [Message Passing Interface (MPI)](https://en.wikipedia.org/wiki/Message_Passing_Interface). For this, we need to setup the build system and we need to initialize our application correctly.

### Build system

The skeleton repository that we provided for you needs to augmented with the configuration for MPI. The toplevel `meson.build` needs the following statements:
```meson
mpi = dependency('mpi', language: 'cpp')
```
You then need to add `mpi` as a dependency to your build targets using the optional `dependencies` argument. This will then enable MPI for your project. This means from now on all your code is compiled with the MPI compiler wrapper `mpicc`, `mpicxx` or `mpic++` that takes care of linking you program to the correct MPI libraries. You can still run your program (`milestone08`) in a normal fashion (as you have done in the past milestones); this is equivalent to running on a single process via `mpirun -n 1 ./milestone08`.

### Running MPI codes

For running your code in parallel, you need to start the executable throuhg `mpirun`. For example on 4 processes, you have to run `mpirun -n 4 ./milestone08` from the command line. Note that if you have less than 4 CPUs in your computer, `mpirun` may not allow you to run your code on 4 processes, but this may still be useful for testing purposes (although you won't see any speedup). You can force `mpirun` execute your command by specifying the `--oversubscribe` option: `mpirun -n 4 --oversubscribe ./milestone08`

### Preparing your code for MPI

Before you can execute any MPI commands in your own code, you need to initialize MPI. You `main` needs to have the following structure:
```c++
#include "mpi.h"

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    ...
    MPI_Finalize();
}
```
MPI communication can be used in between `MPI_Init` and `MPI_Finalize`.

### MPI support module

MPI does not officially have C++ bindings. The C bindings are described for example [here](https://www.open-mpi.org/doc/current/). You will have to use the C bindings for your code, but we provide a simple C++ support module that eases usage of the C library. One particular advantage is that we can overload functions in C++, providing functionality for different data types while in C you have to specify the data type directly. The C++ support module can be download here: [mpi_support.h](mpi_support.h)

As an example, [mpi_support.h](mpi_support.h) contains a wrapper for the [`MPI_Allreduce`](https://www.open-mpi.org/doc/current/man3/MPI_Allreduce.3.php) function that we will use to compute sums across all MPI processes, for example to sum up the energy contributions. A bare C implementation for summing the kinetic energy could look like this:
```c
double ekin_total = 0;  // sum of total kinetic energy
double ekin_local;  // kinetic energy on current process
epot_local = kinetic_energy(...);
MPI_Allreduce(&ekin_local, 1, MPI_DOUBLE, &ekin_total, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
// ekin_total now contains the sum
```
The wrappers in the support module deduce the type automatically, allowing you to write
```c++
double ekin_total{MPI::allreduce(kinetic_energy(...), MPI_SUM, MPI_COMM_WORLD)};
// ekin_total now contains the sum
```
This leads to more compact (and more readable) code. It is also less error prone as the C++ wrapper makes sure the right MPI type (`MPI_DOUBLE`, `MPI_INT`, etc.) is passed to the underlying communication function.

The header [mpi_support.h](mpi_support.h) also contains helper functions for serialization (or "packing") of buffers for subsequent sent operations. Inspect the file [mpi_support.h](mpi_support.h) and try to understand what it does.

## Domain decomposition

We now parallelize the molecular dynamics code using domain decomposition. We provide for you a domain decomposition module that takes care of communicating atoms and ghost atoms. The module implements the `Domain` class and can be downloaded here: [domain.h](domain.h), [domain.cpp](domain.cpp). Inspect these files and try to understand how they work.

So far, our simulation code is agnostic of the spatial domain we are simulating. The gold cluster of Milestone 07 can be located anywhere in space. For the purpose of domain decomposition, we need to specify a bounding box (the simulation domain) in which our atoms live. When atoms leave this bounding box they are either discarded or wrapped periodically back to the opposite end of this box. The `Domain` class is a representation of this bounding box or simulation domain.

The `Domain` class can also subdivide the global domain into an integer number of subdomains. The constructor takes for arguments: The MPI communicator, the length of the domain in the three cartesian direction, the number of subdomains for these directions and whether these directions should be treated periodically. For example
```c++
Domain domain(MPI_COMM_WORLD, {3.5, 4.5, 5.5}, {1, 1, 4}, {0, 0, 1});
```
instantiates a domain a size $$3.5~Å\times 4.5~Å\times 5.5~A$$. The origin is located at position $$0,0,0$$. This domain is subdivided into $$4$$ subdomain in $$z$$-direction. (The code hence needs to be run with $$4$$ MPI processes.) Only the $$z$$-direction is periodic. Note that the `Domain` class is useful even for serial calculations to model periodic domains. In this case, the `Domain` class simply replicates atoms from the simulation domain as ghost atoms.

The `Domain` class knows two states of the simulation. First, a replicated state where each MPI process contains all atoms, i.e. the respective `Atoms` objects are simply replicated across the MPI processes. Second, a decomposed state in which each MPI process only contains the atoms within the subdomain (and possibly ghost atoms). After instantiation, the `Domain` object is in the replicated state. File I/O needs to happen in the replicated state. Files needs to be read on all processes but written only on a single process.

The `Domain` class requires the `Atoms` class to have a `resize` method that resizes all arrays (positions, velocities, etc.) without loosing existing data. You will need to implement this method for your `Atoms` class.

### Enabling/disabling domain decomposition

The `enable` method of the `Domain` class switches into the decomposed state. Before starting the integrator loop, you would execute
```c++
domain.enable(atoms);
```
to switch to the decomposed state. You can switch back to the replicated state by issuing
```c++
domain.disable(atoms);
```
In an integrator loop, you need to disable domain decomposition before writing snapshots to trajectory files using `write_xyz`.

### Moving atoms between subdomains

An atom may leave the current subdomain during an integration step. A call to
```c++
domain.exchange_atoms(atoms);
```
moves these atoms to the neighboring domain.

### Populating ghost atoms

Before computing forces, you need to populate the `Atoms` object with ghost atoms from the neighboring subdomain. Use
```c++
domain.update_ghosts(atoms, border_width);
```
to do this. The parameter `border_width` specifies up to what distance to the subdomain ghost atoms are required. Note that for an EAM potential, the `border_width` needs to be twice the cutoff of the potential. Note that atoms exchange or disabling and subsequent enabling of the domain decomposition removes all ghost atoms.

## Back to the gold cluster

Incorporate domain decomposition into your MD code and run the gold clusters from Milestone 07 on $$1$$, $$2$$, $$4$$ and $$8$$ MPI processes. Show that a pure microcanonical (NVE) run conserves energy.

When modifying your code you need to think about how to compute the potential energy. The implementation of the EAM potential is entirely unaware of the decomposed state of your calculation; it will simply return the potential energy for your subdomain including the ghost atoms. You will need to compute the potential energy for _just_ the atoms in the subdomain, excluding the ghosts. A way to achieve this is to introduce a per-atom potential energy and sum that energy only over the atoms local to the current subdomain. The global potential (and kinetic) energy then needs to be summed over all MPI processes using a reduce operation.

## Task Summary

This milestone requires the following tasks:

* Parallelize the code using the domain-decomposition module provides by us
* Optional: Develop your own domain-decomposition module
* Carry out first parallel runs on the gold clusters from Milestone 07

We ask you to provide and discuss the following figures in your final report:

* Plot of the total energy versus time to demonstrate energy conservation

We provide the following files for you:

* [mpi_support.h](mpi_support.h)
* [domain.h](domain.h)
* [domain.cpp](domain.cpp)
