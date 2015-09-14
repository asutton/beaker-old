// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/evaluate.hpp"
#include "beaker/expr.hpp"


namespace beaker
{


// -------------------------------------------------------------------------- //
//                         Evaluation of expressions


Value 
evaluate(Expr const* e)
{
  struct Evaluate_fn
  {
    Value operator()(Constant_expr const* e) const { return evaluate(e); }
    Value operator()(Identifier_expr const* e) const { return evaluate(e); }
    Value operator()(Unary_expr const* e) const { return evaluate(e); }
    Value operator()(Binary_expr const* e) const { return evaluate(e); }
    Value operator()(Call_expr const* e) const { return evaluate(e); }
  };
  return apply(e, Evaluate_fn());
}


// The value of a constant expression is that value.
Value
evaluate(Constant_expr const* e)
{
  return e->value();
}


// The value of an identifier is determined by its
// initializer.
//
// TODO: This can be defined for constant declarations or
// generalized constant expressions.
Value
evaluate(Identifier_expr const* e)
{
  error(e->location(), "not a constant expression");
  return 0;
}


// The value of a unary expression depends on the operator.
// Note that there are no overflow conditions to worry
// about with these operations.
Value
evaluate(Unary_expr const* e)
{
  switch (e->op()) {
    case num_neg_op:
      return -evaluate(e->arg());
    case num_pos_op:
      return evaluate(e->arg());
    case bit_not_op:
      return ~evaluate(e->arg());
    case log_not_op:
      return !evaluate(e->arg());
    default:
      break;
  }
  lingo_unreachable();
}


// The value of a binary expression depends on the operator.
//
// TODO: Implement checks for undefined behavior:
//
//    - signed integer overflow
//    - division by 0
//    - shift by negative numbers
//    - shift by an amount greater than the LHS width
Value
evaluate(Binary_expr const* e)
{
  switch (e->op()) {
    case num_add_op:
      return evaluate(e->left()) + evaluate(e->right());
    case num_sub_op:
      return evaluate(e->left()) - evaluate(e->right());
    case num_mul_op:
      return evaluate(e->left()) * evaluate(e->right());
    case num_div_op:
      return evaluate(e->left()) / evaluate(e->right());
    case num_mod_op:
      return evaluate(e->left()) % evaluate(e->right());
    case bit_and_op:
      return evaluate(e->left()) & evaluate(e->right());
    case bit_or_op:
      return evaluate(e->left()) | evaluate(e->right());
    case bit_xor_op:
      return evaluate(e->left()) ^ evaluate(e->right());
    case bit_lsh_op:
      return evaluate(e->left()) << evaluate(e->right());
    case bit_rsh_op:
      return evaluate(e->left()) >> evaluate(e->right());
    case rel_eq_op:
      return evaluate(e->left()) == evaluate(e->right());
    case rel_ne_op:
      return evaluate(e->left()) != evaluate(e->right());
    case rel_lt_op:
      return evaluate(e->left()) < evaluate(e->right());
    case rel_gt_op:
      return evaluate(e->left()) > evaluate(e->right());
    case rel_le_op:
      return evaluate(e->left()) <= evaluate(e->right());
    case rel_ge_op:
      return evaluate(e->left()) >= evaluate(e->right());
    case log_and_op:
      return evaluate(e->left()) && evaluate(e->right());
    case log_or_op:
      return evaluate(e->left()) || evaluate(e->right());
    default:
      break;
  }
  lingo_unreachable();
}


// The value of a call expression is computed by the function's
// definition.
//
// TODO: Implementing this leads to generalized constant 
// expressions (essentially as defined in C++14).
Value
evaluate(Call_expr const* e)
{
  error(e->location(), "'{}' is not a constant expression");
  return 0;
}


// -------------------------------------------------------------------------- //
//                         Reduction of expressions

// Returns true if the expression is fully reduced. Only
// constant expressions are fully reduced.
inline bool
is_reduced(Expr const* e)
{
  return is<Constant_expr>(e);
}


// Reduce the given expression.
Expr const* 
reduce(Expr const* e)
{
  struct Reduce_fn
  {
    Expr const* operator()(Constant_expr const* e) const { return reduce(e); }
    Expr const* operator()(Identifier_expr const* e) const { return reduce(e); }
    Expr const* operator()(Unary_expr const* e) const { return reduce(e); }
    Expr const* operator()(Binary_expr const* e) const { return reduce(e); }
    Expr const* operator()(Call_expr const* e) const { return reduce(e); }
  };
  return apply(e, Reduce_fn());
}


// A constant expression is fully reduced.
Expr const*
reduce(Constant_expr const* e)
{
  return e;
}


// An identifier cannot be reduced.
//
// TODO: This is not true. However, we need constant 
// declarations or generalized constant expressions to 
// "see through" identifiers.
Expr const*
reduce(Identifier_expr const* e)
{
  return e;
}


// A unary expression can be reduced only if its operand
// can be fully reduced.
Expr const*
reduce(Unary_expr const* e)
{
  Expr const* e1 = reduce(e->arg());
  Expr const* r = make_unary_expr(e->location(), e->op(), e1);

  // If the operand is reduced, then fold this operation.
  if (is_reduced(e1))
    return make_constant_expr(r->location(), r->type(), evaluate(r));
  else
    return r;
}


// A binary expression can be reduced only if its operand
// can be fully reduced.
Expr const*
reduce(Binary_expr const* e)
{
  Expr const* e1 = reduce(e->left());
  Expr const* e2 = reduce(e->right());
  Expr const* r = make_binary_expr(e->location(), e->op(), e1, e2);

  // If the expression can be folded, fold it.
  if (is_reduced(e1) && is_reduced(e2))
    return make_constant_expr(r->location(), r->type(), evaluate(r));
  else
    return r;
}


// A function call cannot be reduced.
//
// TODO: This is not true. If all of the arguments can be 
// reduced then we can substitute through. This is the same 
// as generalized constant expressions.
Expr const*
reduce(Call_expr const* e)
{
  return e;
}




} // namespace beaker
