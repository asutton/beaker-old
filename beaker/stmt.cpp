// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "decl.hpp"
#include "expr.hpp"
#include "type.hpp"
#include "stmt.hpp"


namespace beaker
{

Location
Declaration_stmt::location() const
{
  return decl()->location();
}


Location
Expression_stmt::location() const
{
  return expr()->location();
}


Location
Assignment_stmt::location() const
{
  return lhs()->location();
}



// -------------------------------------------------------------------------- //
//                             Statement builders

Empty_stmt*
make_empty_stmt(Location loc)
{
  return new Empty_stmt(loc);
}


Declaration_stmt* 
make_declaration_stmt(Decl const* d)
{
  return new Declaration_stmt(d);
}


Expression_stmt* 
make_expression_stmt(Location loc, Expr const* e)
{
  return new Expression_stmt(loc, e);
}


Assignment_stmt* 
make_assign_stmt(Location loc, Expr const* e1, Expr const* e2)
{
  return new Assignment_stmt(loc, e1, e2);
}



namespace
{

// Diagnose errors in a condition.
bool
check_condition(Expr const* e) 
{
  // FIXME: Define and use the span to diagnose this error.
  if (!is_boolean_type(e->type())) {
    error(e->location(), "expression does not have type 'bool'");
    return false;
  }
  return true;
}


} // namespace


If_then_stmt*
make_if_then_stmt(Location loc, Expr const* e, Stmt const* s)
{
  if (!check_condition(e))
    return make_error_node<If_then_stmt>();
  return new If_then_stmt(loc, e, s);
}


If_else_stmt*
make_if_else_stmt(Location l1, Location l2, Expr const* e, Stmt const* s1, Stmt const* s2)
{
  if (!check_condition(e))
    return make_error_node<If_else_stmt>();
  return new If_else_stmt(l1, l2, e, s1, s2);
}


While_stmt*
make_while_stmt(Location loc, Expr const* e, Stmt const* s)
{
  if (!check_condition(e))
    return make_error_node<While_stmt>();
  return new While_stmt(loc, e, s);
}


Do_stmt*
make_do_stmt(Location l1, Location l2, Expr const* e, Stmt const* s)
{
  if (!check_condition(e))
    return make_error_node<Do_stmt>();
  return new Do_stmt(l1, l2, e, s);
}


Return_stmt*
make_return_stmt(Location l1, Location l2, Expr const* e)
{
  return new Return_stmt(l1, l2, e);
}


Block_stmt* 
make_block_stmt(Location l1, Location l2, Stmt_seq const& seq)
{
  return new Block_stmt(l1, l2, seq);
}


File_stmt* 
make_file_stmt(Stmt_seq const& seq)
{
  return new File_stmt(seq);
}



} // namespace beaker

