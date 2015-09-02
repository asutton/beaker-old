// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/token.hpp"
#include "beaker/lexer.hpp"
#include "beaker/parse.hpp"

#include "lingo/file.hpp"

#include <iostream>


using namespace lingo;
using namespace beaker;

int
main(int argc, char* argv[])
{
  init_tokens();

  if (argc < 2) {
    error("invalid arguments");
    return -1;
  }

  // Open the input file.
  File& f = open_file(argv[1]);
  Input_context cxt(f);
  
  Token_list toks = lex(f);
  if (error_count())
    return -1;

  Stmt const* prog = parse(toks);
  if (error_count())
    return -1;

  // TODO: Print something?
  (void)prog;
  std::cout << "OK\n";
}
