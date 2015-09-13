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
  unit.cpp
  operator.cpp
  variable.cpp
  function.cpp
  lookup.cpp
  less.cpp
  same.cpp
  print.cpp
  token.cpp
  lexer.cpp
  parse.cpp
  parse-type.cpp
  parse-expr.cpp
  parse-decl.cpp
  parse-stmt.cpp
  llvm.cpp)
