// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_EVALUATE_HPP
#define BEAKER_EVALUATE_HPP

// This module provides support for compile-time evaluation
// of expressions (i.e., constant folding). Evlauation takes
// an expression and produces a value. Reduction takes an
// expression and produces the smallest expression that would
// evaluate to same value as the original.
//
// TODO: Support evaluation of statements for full compile-time
// evaluation support.

#include "beaker/prelude.hpp"
#include "beaker/value.hpp"


namespace beaker
{

Value evaluate(Expr const*);
Value evaluate(Constant_expr const*);
Value evaluate(Identifier_expr const*);
Value evaluate(Unary_expr const*);
Value evaluate(Binary_expr const*);
Value evaluate(Call_expr const*);

Expr const* reduce(Expr const*);
Expr const* reduce(Constant_expr const*);
Expr const* reduce(Identifier_expr const*);
Expr const* reduce(Unary_expr const*);
Expr const* reduce(Binary_expr const*);
Expr const* reduce(Call_expr const*);


} // namespace beaker


#endif

