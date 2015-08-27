# Copyright (c) 2015 Andrew Sutton
# All rights reserved

macro(add_test_driver target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} steve lingo)
endmacro()

# Test programs
add_test_driver(test-types types.cpp)
add_test_driver(test-exprs exprs.cpp)
add_test_driver(test-var var.cpp)


# Actual unit tests.
add_test(test-types test-types)
add_test(test-exprs test-exprs)

