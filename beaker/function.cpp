// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/function.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/decl.hpp"
#include "beaker/stmt.hpp"
#include "beaker/same.hpp"


namespace beaker
{

// -------------------------------------------------------------------------- //
//                            Return type checking

namespace
{

Type const* check_return(Type const*, Stmt const*);


Type const*
check_return(Type const* t, Block_stmt const* s)
{
  Type const* r = nullptr;
  for (Stmt const* s1 : s->statements()) {
    Optional<Type> t1 = check_return(t, s1);

    // Only update the return value if we find a definite
    // return type. Otherwise, it stays null. Also, don't
    // Don't "forget" if we got an error.
    if (!t1)
      r = make_error_node<Type>();
    else if (!is_error_node(r) && *t1)
      r = *t1;
  }
  return r;
}


Type const*
check_return(Type const* t, If_then_stmt const* s)
{
  return check_return(t, s->branch());
}


Type const*
check_return(Type const* t, If_else_stmt const* s)
{
  Optional<Type> t1 = check_return(t, s->true_branch());
  Optional<Type> t2 = check_return(t, s->false_branch());
  
  // Return t iff either branch has a return statement.
  if (t1 && t2)
    return (*t1 || *t1) ? t : nullptr;
  else
    return make_error_node<Type>();
}


Type const*
check_return(Type const* t, While_stmt const* s)
{
  return check_return(t, s->body());
}


Type const*
check_return(Type const* t, Do_stmt const* s)
{
  return check_return(t, s->body());
}


// For an exit statmeent, the result type must be `void`.
Type const*
check_return(Type const* t, Exit_stmt const* s)
{
  if (!is_void_type(t)) {
    error(s->location(), "no return value in non-void function");
    return make_error_node<Type>();
  }
  return t;
}


// Check that the type of the result type is the same
// as `t`.
//
// TODO: Support implicit conversion to the return type.
Type const*
check_return(Type const* t, Return_stmt const* s)
{
  Expr const* e = s->result();
  Type const* r = e->type();

  // The type of the return statement shall match the
  // declared type of the function
  if (!same(t, r)) {
    // Generate different error messages depending on
    // the mismatched types.
    if (is_void_type(t))
      error(e->location(), "returning a value from a 'void' function");
    else
      error(e->location(), "returning a value of type '{}'", r);
    return make_error_node<Type>();
  }
  
  return t;
}


// Dispatch to an approp
struct Check_return_fn
{
  Check_return_fn(Type const* t)
    : t(t)
  { }

  // These do not return, so they are trivially satisfied.
  //
  // TODO: Use a concept to declare these to be non-compound.
  Type const* operator()(Empty_stmt const* s) const { return t; }
  Type const* operator()(Declaration_stmt const* s) const { return t; }
  Type const* operator()(Expression_stmt const* s) const { return t; }
  Type const* operator()(Assignment_stmt const* s) const { return t; }

  // Recursively check compound statements.
  Type const* operator()(If_then_stmt const* s) const { return check_return(t, s); }
  Type const* operator()(If_else_stmt const* s) const { return check_return(t, s); }
  Type const* operator()(While_stmt const* s) const { return check_return(t, s); }
  Type const* operator()(Do_stmt const* s) const { return check_return(t, s); }
  Type const* operator()(Block_stmt const* s) const { return check_return(t, s); }
  
  // The main event...
  Type const* operator()(Exit_stmt const* s) const { return check_return(t, s); }
  Type const* operator()(Return_stmt const* s) const { return check_return(t, s); }

  Type const* t;
};


// Check the return type of a statement. This returns `t` if
// the statement has a return type and nullptr if it does not.
// An error node is returned if an error was diagnosed during
// processing.
Type const*
check_return(Type const* t, Stmt const* s)
{
  return apply(s, Check_return_fn(t));
}


} // namespace


// Walk through the sequence of statements looking for return
// statements. The type of the returned expression shall match
// the declared return type of the function.
//
// A function body has no return statements, the function shall
// have a declared return type of void.
bool
check_definition(Type const* t, Stmt const* s)
{
  // If no return type was found, then the return type
  // of the function must be void.
  Optional<Type> r = check_return(t, s);
  if (!r)
    return false;
  if (!*r && t != get_void_type()) {
    error("no return value in non-void function");
    return false;
  }
  return true;
}


// Check that the return type of a function definition
// conforms to the function declaration.
bool
check_definition(Function_decl const* f, Stmt const* s)
{
  return check_definition(f->return_type(), s);
}


// Check that the types of function arguments match those
// of the declared parameters.
bool
check_arguments(Function_type const* t, Expr_seq const& args)
{
  Type_seq const& parms = t->parameter_types();

  // Make sure the number of parameters and arguments agree.
  int nparms = parms.size();
  int nargs = args.size();
  if (nparms != nargs) {
    if (nparms < nargs)
      error("too many arguments (expected {} but got {})", nparms, nargs);
    else if (nparms > nargs)
      error("too few arguments (expected {} but got {})", nparms, nargs);
    return false;
  }

  // Check that the argument types match the parameter types.
  int nerr = 0;
  for (int i = 0; i < nargs; ++i) {

    Type const* p = parms[i];
    Type const* a = args[i]->type();

    if (!same(p, a)) {
      Expr const* e = args[i];
      error(e->location(), "type mismatch in argument {} "
                           "(expected '{}' but got '{}')", i + 1, p, a);
      ++nerr;
    }
  }
  return nerr == 0;
}


} // namespace beaker
