// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_PRINT_HPP
#define BEAKER_PRINT_HPP

// This module provides support for pretty printing
// terms in the language.

#include "beaker/prelude.hpp"

#include "lingo/print.hpp"


namespace beaker
{

void print(Printer&, Type const*);
void print(Printer&, Void_type const*);
void print(Printer&, Boolean_type const*);
void print(Printer&, Integer_type const*);
void print(Printer&, Function_type const*);
void print(Printer&, Reference_type const*);

void print(Printer&, Expr const*);
void print(Printer&, Constant_expr const*);
void print(Printer&, Identifier_expr const*);
void print(Printer&, Unary_expr const*);
void print(Printer&, Binary_expr const*);
void print(Printer&, Call_expr const*);

void print(Printer&, Decl const*);
void print(Printer&, Variable_decl const*);
void print(Printer&, Function_decl const*);
void print(Printer&, Parameter_decl const*);

void print(Printer&, Stmt const*);
void print(Printer&, Empty_stmt const*);
void print(Printer&, Declaration_stmt const*);
void print(Printer&, Expression_stmt const*);
void print(Printer&, Assignment_stmt const*);
void print(Printer&, If_then_stmt const*);
void print(Printer&, If_else_stmt const*);
void print(Printer&, While_stmt const*);
void print(Printer&, Do_stmt const*);
void print(Printer&, Return_stmt const*);
void print(Printer&, Block_stmt const*);

void print(Printer&, Unit const*);


inline std::ostream& 
operator<<(std::ostream& os, Type const* t)
{
  return os << to_string(t);
}


} // namespace beaker


#endif

