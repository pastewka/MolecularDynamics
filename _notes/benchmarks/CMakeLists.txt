cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(cpp_hpc_examples)

# Try to find Eigen if it is installed
find_package(Eigen3 3.4.0 EXACT NO_MODULE)

# If not installed, fetch Eigen from the web
if(NOT Eigen3_FOUND)
  set(EIGEN_BUILD_TESTING OFF CACHE BOOL "Turn off tests" FORCE)

  FetchContent_Declare(
    Eigen3
    URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
    URL_HASH SHA256=8586084f71f9bde545ee7fa6d00288b264a2b7ac3607b974e54d13e7162c1c72
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )

  # We are not using FetchContent_MakeAvailable since that pulls in all
  # of Eigen's tests
  FetchContent_GetProperties(Eigen3)
  if (NOT Eigen3_POPULATED)
    FetchContent_Populate(Eigen3)  # Downloading Eigen
  endif()

  message(STATUS "Populated Eigen3 directory: ${eigen3_SOURCE_DIR}")

  # Setting up search of Eigen
  set(CMAKE_MODULE_PATH ${eigen3_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
  set(CMAKE_PREFIX_PATH ${eigen3_SOURCE_DIR} ${CMAKE_PREFIX_PATH})

  # Use FindEigen3.cmake from Eigen distribution
  find_package(Eigen3 3.4.0 REQUIRED MODULE EXACT)
endif()

# Adding local version of benchmark
set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_Declare(
  benchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.8.0.tar.gz
  URL_HASH SHA256=ea2e94c24ddf6594d15c711c06ccd4486434d9cf3eca954e2af8a20c88f9f172
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(benchmark)

add_executable(bench gemv_benchmarks.cpp)
target_link_libraries(bench Eigen3::Eigen benchmark::benchmark)
