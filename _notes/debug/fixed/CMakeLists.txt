cmake_minimum_required(VERSION 3.0)
project(cpp_hpc_examples_fixed)

# Link to eigen (also sets the includes)
find_package(Eigen3 3.4.0 REQUIRED NO_MODULE)
link_libraries(Eigen3::Eigen)

# Set -std=c++20 (needed for std::span)
set(CMAKE_CXX_STANDARD 20)

# Activate sensible warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# Activate AddressSanitizer
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(nullpointer nullpointer.cpp)
add_executable(localaddress localaddress.cpp)
add_executable(outofbounds outofbounds.cpp)
add_executable(signedintegers signedintegers.cpp)
