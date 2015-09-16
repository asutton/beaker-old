// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/decl.hpp"
#include "beaker/expr.hpp"
#include "beaker/type.hpp"
#include "beaker/stmt.hpp"
#include "beaker/same.hpp"

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
make_assignment_stmt(Location loc, Expr const* e1, Expr const* e2)
{
  if (Reference_type const* t = as<Reference_type>(e1->type())) {
    if (is_object_type(t->type())) {
      // The expression types of the operands shall match.
      //
      // TODO: Support implicit conversion.
      Type const* t1 = get_expr_type(e1);
      Type const* t2 = get_expr_type(e2);
      if (!same(t1, t2)) {
        error(e2->location(), "type mismatch in assigned value "
                              "(expected '{}' but got '{}')",
                              t1, t2);
        return make_error_node<Assignment_stmt>();
      }
      return new Assignment_stmt(loc, e1, e2);
    }

    // TODO: Can you assign to a function? Like this:
    //
    //    var x : (int) -> int = f1;
    //    x = f2; // OK?
    //
    // That seems like a bit of stretch.

    error(e1->location(), "assignment to non-object");
    return make_error_node<Assignment_stmt>();
  } else {
    // TODO: Diagnose the span of the LHS?
    if (is_void_type(e1->type()))
      error(e1->location(), "assignment to 'void'");
    else
      error(e1->location(), "assignment to temporary");
  }
  return make_error_node<Assignment_stmt>();
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


Exit_stmt*
make_exit_stmt(Location l1, Location l2)
{
  return new Exit_stmt(l1, l2);
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


} // namespace beaker

