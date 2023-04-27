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

Put these files in a folder **seperate** from the class project.

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

## Debugging

Many bugs can be caught by enabling warnings and AddressSanitizer at compile
time as shown above and running the program. However, this will most times only
show the point of crash of your program, not where the bug actually is. To find
where the bug is, it is very useful to use a debugger like the GNU Debugger
(GDB). We'll now explain the basic workflow with GDB.

### Compiling

Make sure you ran CMake in `Debug` mode:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```
This turns *off* code optimizations (`-O0` compiler flag) and adds debugging
symbols to the executable (`-g` compiler flag), which are both necessary to
properly debug. Additionally, the macro `NDEBUG` is not defined, which enables
the `assert()` function from the standard header `<cassert>` and enables bounds
check in Eigen arrays. All these features will make your life easier when
writing code.

### Running GDB

We will run GDB with a Terminal User Interface (TUI):

```bash
gdb --tui ./nullpointer
```

The top window should show you the source code (if not, make sure you compile
with `-DCMAKE_BUILD_TYPE=Debug`). If the top windows is highlighted with a blue
border, you can use the arrow keys to navigate the code. Use the shortcut `C-x
o` (Ctrl-X then O), to alternate focus between the top and bottom window. If the
focus is not on the top window, the arrow keys will cycle the command history
(similar to arrows in bash). If you want to enable command history you can add
`set history save on` to `~/.gdbinit`.

### Manipulating program state

You can use the `run` command to run the program until the crash. The source
window will then show where the crash occurs, and why, but that's often not
where the bug is. You can still use `up` to see which call of `set_to_one` has
the crash. Using `start` you can execute the program from the very beginning,
line by line.

Use the `print` command to show the value of the `pointer` variable. Since it is
tedious to `print` at every instruction, use `watch pointer`, then `next` to
advance the program execution. The `watch` command will show when `pointer`
changes value. When hitting enter without a command, it just repeats the last
command. Advance the program until `pointer` has the value `0x0` and fix the
bug.

Notable other GDB commands:

- `info locals`: show all variables in current stack
- `print <var>`: show value of variable
- `display <var>`: show value of variable at every step
- `watch <var>`: show when variable changes value
- `backtrace`: show the call stack
- `up`: move up the call stack
- `down`: move down the call stack
- `break <file>:<line>`: put a breakpoint at given line in a file
- `break <function>`: breakpoint when a function is called
- `continue`: resume execution until breakpoint, crash or end of program
- `finish`: resume execution until current function returns
- `until`: resume execution until a source line "greater" than the current
  location---essentially until the end of a scope (loop, if statement, etc.)

### Memory leaks

After having fixed the null pointer bug, activate the AddressSanitizer (see
above) and run the program. It should show a memory leak of 4 bytes.
AddressSanitizer can also be used for the two other examples (localaddress.cpp
and outofbounds.cpp), usually showing the call stack (backtrace) at the moment
of the crash.

## Fixed files

Below are versions of the files above where we used idiomatic C++, the standard
library and Eigen instead of C++-flavoured C:

- [CMakeLists.txt](debug/fixed/CMakeLists.txt)
- [nullpointer.cpp](debug/fixed/nullpointer.cpp)
- [localaddress.cpp](debug/fixed/localaddress.cpp)
- [outofbounds.cpp](debug/fixed/outofbounds.cpp)
- [signedintegers.cpp](debug/fixed/signedintegers.cpp)
