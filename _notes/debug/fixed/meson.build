project(
    'Debug example', # Project name
    ['c', 'cpp'], # Project type. We need a C++ compiler. The C compiler is required for MPI.
    default_options : [
        'cpp_std=c++20',  # Needed for std::span
        'warning_level=3',  # -Wall -Wextra -Wpedantic
    ],
    version : '0.1'
)

add_project_arguments('-fsanitize=address', language : ['c', 'cpp'])
add_project_link_arguments('-fsanitize=address', language : ['c', 'cpp'])

eigen = dependency('eigen3', version : '>=3.4.0')

executable(
  'nullpointer',
  'nullpointer.cpp',
  dependencies : [eigen]
)

executable(
  'localaddress',
  'localaddress.cpp',
  dependencies : [eigen]
)

executable(
  'outofbounds',
  'outofbounds.cpp',
  dependencies : [eigen]
)

executable(
  'signedintegers',
  'signedintegers.cpp',
  dependencies : [eigen]
)
