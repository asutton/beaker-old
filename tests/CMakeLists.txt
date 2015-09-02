# Copyright (c) 2015 Andrew Sutton
# All rights reserved

macro(add_test_driver target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} beaker lingo)
endmacro()

set(INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/input)

# Test programs
add_test_driver(test-types types.cpp)
add_test_driver(test-exprs exprs.cpp)
add_test_driver(test-var   var.cpp)
add_test_driver(test-lex   lex.cpp)
add_test_driver(test-parse parse.cpp)


# Actual unit tests.
add_test(test-types test-types)
add_test(test-exprs test-exprs)
add_test(test-lex   test-lex ${INPUT_DIR}/lex/1.bkr)
