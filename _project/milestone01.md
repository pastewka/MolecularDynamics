---
layout: default
title:  "Milestone 01"
parent: Project
categories: project
author : Lars Pastewka
nav_order: 2
---

# Milestone 1 <br/> Setting up the build environment

## Learning goals

The student will...
* ...be able to compile a C++ project with Meson.
* ...be able to specify build types of a Meson project.

## Introduction

In this first milestone we will set up a build environemnt and make sure
that you can compile C++ code on your computer. This will be the starting
point for the developments in the following milestones.

## Setting up your system

The starting point of your project is setting up a proper build environment.
This means, you need to install and test all tools necessary for the project.
In particular, you will need:
* A C++ compiler
* [Meson](https://mesonbuild.com) (at least version 0.54.0) for our build environment
* An [MPI](https://en.wikipedia.org/wiki/Message_Passing_Interface) installation

We provide installation instructions for [Ubuntu](https://ubuntu.com/)
installations. If you have a Windows machine, we recommend to use
the [Windows Subsystem for Linux (WSL)](https://en.wikipedia.org/wiki/Windows_Subsystem_for_Linux).
Documentation on how to install WSL on Windows 10 can be found
[here](https://docs.microsoft.com/en-us/windows/wsl/install-win10). The following
instructions also apply for Ubuntu installed within WSL.

Once you have these things set up, open a command shell and type
```bash
sudo apt-get update
sudo apt-get install gcc make meson ninja-build gdb valgrind mpi-default-dev
```
Note that if you are on a different system than Ubuntu, these commands may differ.
On [Ubuntu](https://ubuntu.com/)/[Debian](https://www.debian.org/) it is `apt` or `apt-get`
but on [Fedora](https://getfedora.org/)/[CentOS](https://www.centos.org/)/[RHEL](https://www.redhat.com/en/technologies/linux-platforms/enterprise-linux)
the package manager command is `dnf`. On macOS it depends on which package manager you have installed, the
most popular one is [Homebrew](https://brew.sh/) which provides the `brew` command for
package installation. The names of the packages will also vary between these systems.

We recommend using a development environment for developing code. We
ourselves use [CLion](https://www.jetbrains.com/clion/). Free educational
licenses for CLion can be obtained [here](https://www.jetbrains.com/community/education/).
CLion is available for all of the above platforms and can on Windows be configured
to use WSL. (Documentation on CLion and WSL can be found [here](https://www.jetbrains.com/help/clion/how-to-use-wsl-development-environment-in-product.html).)

## Creating an empty repository

The first thing you need to do is to set up your build environment. We have
done this for you and provide a template repository here:
[https://github.com/imtek-simulation/meson-skeleton/](https://github.com/imtek-simulation/meson-skeleton/).
We will now be walking you through the process of obtaining this skeleton repository
from the command line. You can also carry out the whole process within CLion.

On [Github](github.com), you can simply create a new repository from our template. Navigate to
the link above and click on "Use this template". You will be asked for a new name of
the repository: Let's call this yamd (Yet Another Molecular Dynamics code). Since my
username is "pastewka", the repository now resides under

     https://github.com/pastewka/yamd

You can now check this repository out, i.e. copy it to your local machine. On the shell,
type
```bash
git clone git@github.com:pastewka/yamd.git
```
(and replace "pastewka" and "yamd" with whatever is appropriate for you). The
code now resides in the subdirectory "yamd". Note that if you do not want to work
on github, you can also directly check out the template repository.

Our template repository has existing Meson files. Those Meson files are set up
to automatically download the libraries

* [Eigen](https://eigen.tuxfamily.org/) for basic (array) data structures and
* [Googletest](https://github.com/google/googletest) as a testing framework.

We will work with these libraries throughout this class.

_Note_: If you are unfamiliar with the unix shell, we recommend
[this tutorial](https://swcarpentry.github.io/shell-novice/). We also strongly
recommend that you use version control (git). If you are unfamiliar with git
and want to learn more, see
[this tutorial](https://swcarpentry.github.io/git-novice/).

### Compiling the code

At the root of your repository is a README file with the compilation
instructions. It also describes in details how the repository is laid out and
where you should add new files.

For now, follow the instructions to compile the code, run the first milestone
executable and run the tests. Please refer to this README when you wish to add
new files and push them to GitHub.

## Task summary

This milestone requires the following tasks:

* Create a repository for your code development from our template.
* Carefully read the `README.md` file in your fresh code repository.
* Compile the template, run first milestone executable and tests, for the
  following configuration options:
  - `--buildtype=debug`
  - `--buildtype=release`

We provide the following files for you:

* Skeleton repository at [https://github.com/imtek-simulation/meson-skeleton/](https://github.com/imtek-simulation/meson-skeleton/)
