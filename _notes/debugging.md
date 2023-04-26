---
layout: default
title:  "Debugging"
parent: Notes
categories: notes
author : Lucas Frérot
nav_order: 3
---

# Debugging code in C++

Recognizing and debugging errors is a fundamental skill when programming in C++,
as is writing safe, expressive, and modern C++. We will cover the basics in this note.

## Prerequisites

Make sure you have a full GCC, GDB and CMake installation:

```bash
sudo apt install g++ gdb cmake
gdb --tui # <- this should not give an error message
```

We'll work on the following files, which you can now download:

- [CMakeLists.txt](debug/CMakeLists.txt)
- [nullpointer.cpp](debug/nullpointer.cpp)
- [localaddress.cpp](debug/localaddress.cpp)
- [outofbounds.cpp](debug/outofbounds.cpp)
- [signedintegers.cpp](debug/signedintegers.cpp)

## Compile

Run the standard CMake workflow to get started:

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# To compile after changes to sources:
make  # reruns cmake if needed

# To change build type
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

More information on `CMAKE_BUILD_TYPE`: [Default and Custom
Configurations](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#default-and-custom-configurations).

### Compiling other files

You'll notice that `localaddress.cpp` and `outofbounds.cpp` are not compiled. To
create executables, add these lines at the end of `CMakeLists.txt` and compile
again.

```cmake
add_executable(localaddress localaddress.cpp)
add_executable(outofbounds outofbounds.cpp)
```

### Adding compilation flags

To add compilation flags for all targets:

```cmake
add_compile_options(-Wall)                     # activates bare minimum warnings
add_compile_options(-Wall -Wextra -Wpedantic)  # recommended
```

The [AddressSanitizer (Asan)](https://clang.llvm.org/docs/AddressSanitizer.html)
needs compilation **and** link flags:

```cmake
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
```
