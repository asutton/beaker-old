// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "llvm.hpp"

#include "beaker/expr.hpp"


namespace beaker
{

// -------------------------------------------------------------------------- //
//                     Translation to LLVM statements

void llvm_expr(Printer&, Expr const*);


void
llvm_expr(Printer& p, Expr const* e)
{
  if (!is<Constant_expr>(e)) {
    error(e->location(), "non-constant expressions not implemneted!");
    print(p, 0);
  }

  Constant_expr const* c = cast<Constant_expr>(e);
  print(p, c->value());
}


} // namespace beaker
