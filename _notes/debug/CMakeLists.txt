cmake_minimum_required(VERSION 3.0)
project(cpp_hpc_examples)

# Link to eigen (also sets the includes)
find_package(Eigen3 3.4.0 REQUIRED NO_MODULE)
link_libraries(Eigen3::Eigen)

add_compile_options(-Wall -Wextra -Wpedantic -Wsign-conversion)

add_executable(nullpointer nullpointer.cpp)
