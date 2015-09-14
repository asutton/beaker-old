// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/expr.hpp"
#include "beaker/type.hpp"
#include "beaker/decl.hpp"
#include "beaker/same.hpp"
#include "beaker/function.hpp"

namespace beaker
{

String
Expr::node_name() const
{
  return type_str(*this);
}


Identifier_expr::Identifier_expr(Location loc, Decl const* decl)
  : Expr(loc, get_reference_type(decl->type())), decl_(decl)
{ }


String const*
Identifier_expr::name() const
{
  return decl()->name();
}


// Create boolean expression with value `b`.
Constant_expr*
make_bool_expr(Location loc, bool b)
{
  return new Constant_expr(loc, get_bool_type(), b);
}


// Create an integer expression with value `n`.
Constant_expr*
make_int_expr(Location loc, std::intmax_t n)
{
  return new Constant_expr(loc, get_int_type(), n);
}


// Create a value expression having type `t`.
Constant_expr*
make_constant_expr(Location loc, Type const* t, Value n)
{
  return new Constant_expr(loc, t, n);
}


// Return an expression that refers to a declaration.
Identifier_expr*
make_identifier_expr(Location loc, Decl const* d)
{
  return new Identifier_expr(loc, d);
}


// Creates a unary expression.
Unary_expr*
make_unary_expr(Location loc, Unary_op op, Expr const* e)
{
  Type const* t = get_type(op, e);
  return new Unary_expr(loc, t, op, e);
}


// Return a type-checked binary expression.
Binary_expr*
make_binary_expr(Location loc, Binary_op op, Expr const* e1, Expr const* e2)
{
  Type const* t = get_type(op, e1, e2);
  return new Binary_expr(loc, t, op,  e1, e2);
}


// Create a new call expression.
Call_expr*
make_call_expr(Location loc, Expr const* f, Expr_seq const& args)
{
  Input_context cxt(loc);

  // Get the type of the function target.
  Function_type const* t = as<Function_type>(get_expr_type(f));
  if (!t) {
    error(loc, "'{}' is not callable");
    return make_error_node<Call_expr>();
  }

  if (!check_arguments(t, args))
    return make_error_node<Call_expr>();

  return new Call_expr(loc, t->return_type(), f, args);
}


// -------------------------------------------------------------------------- //
//                            Queries


// Returns true if an expression has type bool.
bool 
has_boolean_type(Expr const* e)
{
  return same(get_bool_type(), e->type());
}


// Returns true if an expression has type int.
bool 
has_integer_type(Expr const* e)
{
  return same(get_int_type(), e->type());
}


} // namespace beaker

