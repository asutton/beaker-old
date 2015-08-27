// Copyright (c) 2015 Flowgrammable.org
// All rights reserved

#include "decl.hpp"
#include "expr.hpp"
#include "type.hpp"
#include "stmt.hpp"


namespace beaker
{

Location
Decl_stmt::location() const
{
  return decl()->location();
}


Location
Expr_stmt::location() const
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


Decl_stmt* 
make_decl_stmt(Decl const* d)
{
  return new Decl_stmt(d);
}


Expr_stmt* 
make_expr_stmt(Location loc, Expr const* e)
{
  return new Expr_stmt(loc, e);
}


Assignment_stmt* 
make_assign_stmt(Location loc, Expr const* e1, Expr const* e2)
{
  return new Assignment_stmt(loc, e1, e2);
}


Block_stmt* 
make_block_stmt(Location l1, Location l2, Stmt_seq const& seq)
{
  return new Block_stmt(l1, l2, seq);
}


If_then_stmt*
make_if_then_stmt(Location loc, Expr const* e, Stmt const* s)
{
  return new If_then_stmt(loc, e, s);
}


If_else_stmt*
make_if_else_stmt(Location l1, Location l2, Expr const* e, Stmt const* s1, Stmt const* s2)
{
  return new If_else_stmt(l1, l2, e, s1, s2);
}


While_stmt*
make_while_stmt(Location loc, Expr const* e, Stmt const* s)
{
  return new While_stmt(loc, e, s);
}


Do_while_stmt*
make_do_while_stmt(Location l1, Location l2, Expr const* e, Stmt const* s)
{
  return new Do_while_stmt(l1, l2, e, s);
}


Return_stmt*
make_return_stmt(Location l1, Location l2, Expr const* e)
{
  return new Return_stmt(l1, l2, e);
}



} // namespace beaker

