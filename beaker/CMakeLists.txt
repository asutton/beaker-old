# Copyright (c) 2015 Andrew Sutton
# All rights reserved

# Build the core library.
add_library(beaker STATIC
  prelude.cpp
  value.cpp
  type.cpp
  expr.cpp
  decl.cpp
  stmt.cpp
  operator.cpp
  variable.cpp
  function.cpp
  less.cpp
  same.cpp
  print.cpp
  token.cpp
  lexer.cpp
  parser.cpp
  parse-type.cpp
  parse-expr.cpp)
