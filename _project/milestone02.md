---
layout: default
title:  "Milestone 02"
parent: Project
categories: project
author : Lars Pastewka
nav_order: 3
---

# Milestone 2 <br/> Velocity-Verlet integration for a single atom

## Learning goals

The student will...
* ...understand how to work with source and header files.
* ...learn how to add new tests to the testing framework.

## Introduction

In this second milestone you will implement a Velocity-Verlet integrator. You will learn about header and source
files and how to add them to the Meson build system. You will also need to think about *testing*, i.e. how to systematically test an implementation. Testing makes sure your new function works correctly but it also makes
sure that you will notice when you break it in the future. Note that we explain only the very basics of the
relevant C++ features here but link to more detailed explanation.

## Adding a header/source file

We recommend to add the integration functionality into a new module that consists of a header and source file.
The header file typically has the extension `.h` and contains only the signatures (also called interfaces) of
the functions. It is required such that the module can be used from another source file.

Create a file `verlet.h` with the following contents:
```c++
#ifndef __VERLET_H
#define __VERLET_H

void verlet_step1(double &x, double &y, double &z, double &vx, double &vy, double &vz,
                  double fx, double fy, double fz, double timestep);
void verlet_step2(double &vx, double &vy, double &vz, double fx, double fy, double fz,
                  double timestep);

#endif  // __VERLET_H
```
The `#ifndef __VERLET_H` commands are called a [header guard](https://en.wikipedia.org/wiki/Include_guard) and
avoid double inclusion of the file (e.g. in a chain of include statements). Everything that starts with a `#`
is processed by the [preprocessor](https://en.wikipedia.org/wiki/C_preprocessor) which produces the file that
is actually compiled by the C++ compiler.

The `&` symbol in the function signature is a [reference](https://en.wikipedia.org/wiki/Reference_(C%2B%2B)). It tells the
compiler to pass only the memory location of the underlying data rather than copying it. It should be used for large
compound types. (Passing `int`egers or `double` floating point numbers without reference is fine.) Note that arguments
passed as a reference can be modified inside the function, which is why we can update positions and velocities inside
`verlet_step1`.

The corresponding source file `verlet.cpp` should look like this:
```c++
#include "verlet.h"

void verlet_step1(double &x, double &y, double &z, double &vx, double &vy, double &vz,
                  double fx, double fy, double fz, double timestep) {
    ... implement Verlet step1 here ...
}

void verlet_step2(double &vx, double &vy, double &vz, double fx, double fy, double fz,
                  double timestep) {
    ... implement Verlet step2 here ...
}
```
Header and source files have typically extensions `.h` and `.cpp`. On Unix system, you sometimes find `.hh` and `.cc`
for C++ code. C++ headers sometimes have the suffix `.hpp`.

To compile the code, you need to and sources files to `lib_sources` in the
main `meson.build`. From the command-line, you can compile the code by running `meson compile` in the `builddir` directory.

## Adding new test cases

It is important to properly test _any_ implementation. You can even adopt a [test-driven development](https://en.wikipedia.org/wiki/Test-driven_development)
style in which tests are written before the implementation. We also here encourage you to write tests for all parts
of your molecular dynamics code. We will outline possible testing strategies in the respective milestones.

One possible test strategy for numerical code is to compare a numerical solution against a known analytical solution.
We are here solving Newton's equation of motion. A possible analytical solution would be the motion of an atom under a constant force, since this is straightforward to solve.

To add a new test, create a new file to the `tests` subdirectory and add it to the `test_sources` in `meson.build`
in the `tests` directory. You can copy the file `test_hello_world.cpp` as a template. The project uses [Googletest](https://google.github.io/googletest/).
Please browse the documentation and look at the primer on the documentation page.

A _test case_ consists of a number of _assertion_. An assertion defines a certain outcome of the function to be tested.
As an example, let us assume we were writing a test for a \\(sin\\) function. We know for example than the function vanishes
at integer multiples of \\(\pi\\). A test case could look like this:
```c++
TEST(SinTest, IntegerMultiplesOfPi) {
  EXPECT_EQ(sin(0), 0);
  EXPECT_EQ(sin(pi), 0);
  EXPECT_EQ(sin(2+pi), 0);
}
```
Note that instead of `EXCEPT_EQ` you can use `ASSERT_EQ`, which terminates the test at the first failure. Note that a proper
test for the \\(sin\\) function would also need to test intermediate values.

Tests for floating-point number should not be done using equalities as in the above example. This is because floating-point results are subject to rounding errors. More complex numerical schemes (such as the integrator discussed in the next milestone) are additionally subject to numerical (discretization) errors. Any comparison must therefore be carried out with a certain tolerance. Googletest provides the assertion `EXPECT_NEAR` and `ASSERT_NEAR` for this. These assertions take a third argument that specifies the maximum tolerable absolute difference between the two arguments:
```c++
TEST(SinTest, IntegerMultiplesOfPi) {
  EXPECT_NEAR(sin(0), 0, 1e-6);
  EXPECT_NEAR(sin(pi), 0, 1e-6);
  EXPECT_NEAR(sin(2+pi), 0, 1e-6);
}
```

## Implement integrator and tests

Implement the Velocity-Verlet integration and write a test for it by comparing the motion of a single atom under the action of a constant force. You will find useful code snippets in the lecture material.

Note that the test will require a first mini-(molecular)-dynamics simulation. In order to integrate the equations of motion,
you will need to write a loop of the form
```c++
for (int i = 0; i < nb_steps; ++i) {
    std::cout << "Step: " << i << std::endl;
    verlet_step1(args...);
    ... compute forces here ...
    verlet_step2(args...);
}
```
This loop integrates the equation of motion for `nb_steps`  steps.
The main loop of any molecular dynamics simulation code looks like this. We will add bells and whistles to it, but the main
structure will remain the same.

The stream `std::cout` can be used to print something to the screen. You can find the documentation for [`iostream` here](https://en.cppreference.com/w/cpp/header/iostream).

## Task summary

This milestone requires the following tasks:

* Implement the Velocity-Verlet integrator
* Implement a test for the Velocity-Verlet integrator

