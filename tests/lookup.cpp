// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/lookup.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/decl.hpp"
#include "beaker/token.hpp"


#include <iostream>


using namespace lingo;
using namespace beaker;


inline Expr const*
truth()
{
  return make_true_expr();
}


inline Expr const*
falsity()
{
  return make_true_expr();
}


inline Decl const*
make_bool_var(char const* n, Expr const* e)
{
  return make_variable_decl(get_identifier(n), get_bool_type(), e);
}


inline Decl const*
make_bool_var(String const* n, Expr const* e)
{
  return make_variable_decl(n, get_bool_type(), e);
}


// Test for re-declaration.
void
test_1()
{
  Scope gs(global_scope);

  Decl const* d1 = make_bool_var("v1", truth());
  Decl const* d2 = make_bool_var("v1", falsity());

  lingo_assert(declare(d1)); // OK
  lingo_assert(!declare(d2)); // error: previous declaraiton.
}


// Test lookup
void
test_2()
{
  Scope gs(global_scope);

  String const* n = get_identifier("v");
  Decl const* d1 = make_bool_var(n, truth());
  Decl const* d2 = make_bool_var(n, falsity());

  lingo_assert(declare(d1));        // OK
  lingo_assert(lookup(n) == d1);    // OK: finds d1
  {
    Scope ls(local_scope);
    lingo_assert(declare(d2));      // OK: hides d1
    lingo_assert(lookup(n) == d2);  // OK: finds d2
  }
  lingo_assert(lookup(n) == d1);    // OK: finds d1

}


int
main()
{
  test_1();
  test_2();
}
