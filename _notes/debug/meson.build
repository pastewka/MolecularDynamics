project(
    'Debug example', # Project name
    ['c', 'cpp'], # Project type. We need a C++ compiler. The C compiler is required for MPI.
    default_options : ['cpp_std=c++17'], # Yes, we need C++17, at least for constexpr
    version : '0.1'
)

eigen = dependency('eigen3', version : '>=3.4.0')

executable(
  'nullpointer',
  'nullpointer.cpp',
  dependencies : [eigen]
)
