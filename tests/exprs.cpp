// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/expr.hpp"
#include "beaker/less.hpp"
#include "beaker/print.hpp"

#include <iostream>


using namespace lingo;
using namespace beaker;


void 
test_name()
{
  Expr const* e1 = make_bool_expr(true);
  Expr const* e2 = make_int_expr(0);

  std::cout << e1->node_name() << '\n';
  std::cout << e2->node_name() << '\n';
}


void 
test_print()
{
  Expr const* e1 = make_bool_expr(true);
  Expr const* e2 = make_bool_expr(false);
  Expr const* e3 = make_int_expr(0);
  Expr const* e4 = make_int_expr(1);
  Expr const* e5 = make_int_expr(-1);
  print(e1);
  print(e2);
  print(e3);
  print(e4);
  print(e5);
}


int
main()
{
  test_name();
  test_print();
  return 0;
}
