// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/decl.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/token.hpp"
#include "beaker/print.hpp"

#include <iostream>


using namespace lingo;
using namespace beaker;


void 
test_print()
{
  String const* n = get_identifier("v1");
  Type const* t = get_int_type();
  Expr const* e = make_int_expr(0);;
  Decl const* d1 = make_variable_decl(n, t, e);
  print(d1);
}


int
main()
{
  test_print();
}
