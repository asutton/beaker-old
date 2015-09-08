// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/decl.hpp"
#include "beaker/expr.hpp"
#include "beaker/type.hpp"
#include "beaker/variable.hpp"
#include "beaker/function.hpp"
#include "beaker/less.hpp"

#include "lingo/symbol.hpp"
#include "lingo/token.hpp"


namespace beaker
{

String
Decl::node_name() const
{
  return type_str(*this);
}


// -------------------------------------------------------------------------- //
//                             Node definitions


Function_decl::Function_decl(Location loc, String const* n, Type const* t, Decl_seq const& a, Stmt const* b)
  : Decl(loc, n, t), first(a), second(b)
{ 
  lingo_assert(dynamic_cast<Function_type const*>(t));
}


Function_type const* 
Function_decl::type() const
{
  return static_cast<Function_type const*>(Decl::type());
}


Type const*
Function_decl::return_type() const
{
  return type()->return_type();
}


// -------------------------------------------------------------------------- //
//                             Declaration builders

// Make a new variable declaration. 
Variable_decl* 
make_variable_decl(Location loc, String const* n, Type const* t, Expr const* e)
{
  Input_context cxt(loc);
  if (!check_initializer(t, e))
    return make_error_node<Variable_decl>();
  return new Variable_decl(loc, n, t, e);
}


// Make an uninitialized variable declaration. The initializer
// must be assigned later.
Variable_decl* 
make_variable_decl(Location loc, String const* n, Type const* t)
{
  return new Variable_decl(loc, n, t, nullptr);
}


// Make a new function declaration.
Function_decl* 
make_function_decl(Location loc, String const* n, Decl_seq const& p, Type const* r, Stmt const* s)
{
  Input_context cxt(loc);
  if (!check_return_type(r, s))
    return make_error_node<Function_decl>();
  return new Function_decl(loc, n, get_function_type(p, r), p, s);
}


// Make a new parameter declaration.
//
// TODO: Check the type of the parameter.
Parameter_decl*
make_parameter_decl(Location loc, String const* n, Type const* t)
{
  return new Parameter_decl(loc, n, t);
}


} // namespace beaker

