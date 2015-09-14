// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/operator.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/same.hpp"


namespace beaker
{

namespace
{

// The operand of an arithmetic expression shall have type
// int, and the result type is int.
Type const*
expect_type(Expr const* e, Type const* expect, Type const* result)
{
  if (same(get_expr_type(e), expect))
    return result;
  error(e->location(), "invalid operand of type '{}'", e->type());
  return make_error_node<Type>();
}


Type const*
expect_type(Expr const* e1, Expr const* e2, Type const* expect, Type const* result)
{
  bool b1 = same(get_expr_type(e1), expect);
  bool b2 = same(get_expr_type(e2), expect);
  if (b1 && b2)
    return result;
  if (!b1)
    error(e1->location(), "invalid operand of type '{}'", e1->type());
  if (!b2)
    error(e2->location(), "invalid operand of type '{}'", e2->type());
  return make_error_node<Type>();
}


} // namesapce


// Returns the type of a unary expression.
//
// The operand of a unary arithmetic expression (-e, +e, and ~e)
// shall have integer type. The result type the expression is `int`.
//
// The operand of the unary logical expression (!e) shall have 
// boolean type. The result type the expression is `bool`.
Type const*
get_type(Unary_op op, Expr const* e)
{
  Type const* z = get_int_type();
  Type const* b = get_bool_type();
  switch (op) {
    case num_neg_op:
    case num_pos_op: 
    case bit_not_op: 
      return expect_type(e, z, z);
    case log_not_op:
      return expect_type(e, b, b);
    default:
      break;
  }
  lingo_unreachable();
}


// Returns the type of a binary expression. 
//
// The operands of a binary arithmetic expression (e1 + e2, e1 - e2,
// e1 * e2, e1 / e2, e1 % e2, e1 & e2, e1 | e2, e1 ^ e2, e1 << e2,
// and e1 >> e2) shall have integer type. The result type the 
// expression is `int`.
//
// The operands of a binary relational expression (e1 < e2, e1 > e2,
// e1 <= e2, e1 >= e2, e1 == e2, and e1 != e2) shall have integer or 
// boolean type. The result type the expression is `bool`.
//
// The operands of a binary logical expression (e1 && e2 and e1 || e2) 
// shall have boolean type. The result type the expression is `bool`.
Type const*
get_type(Binary_op op, Expr const* e1, Expr const* e2)
{
  Type const* z = get_int_type();
  Type const* b = get_bool_type();
  switch (op) {
    case num_add_op:
    case num_sub_op:
    case num_mul_op:
    case num_div_op:
    case num_mod_op:
    case bit_and_op:
    case bit_or_op:
    case bit_xor_op:
    case bit_lsh_op:
    case bit_rsh_op:
      // Arithmetic expressions have integer opreands and results.
      return expect_type(e1, e2, z, z);
    
    case rel_eq_op:
    case rel_ne_op:
    case rel_lt_op:
    case rel_gt_op:
    case rel_le_op:
    case rel_ge_op:
      // Relational expressions have any type and the result
      // is bool.
      return b;
    
    case log_and_op:
    case log_or_op:
      // Logical expressions have boolean operands and result.
      return expect_type(e1, e2, b, b);
    default:
      break;
  }
  lingo_unreachable();
}


void 
print(Printer& p, Unary_op op)
{
  switch (op) {
    case num_neg_op: return print(p, '-');
    case num_pos_op: return print(p, '+');
    case bit_not_op: return print(p, '~');
    case log_not_op: return print(p, '!');
  }
  lingo_unreachable("invalid unary operator ({})", (int)op);
}


void 
print(Printer& p, Binary_op op)
{
  switch (op) {
    case num_add_op: return print(p, '+');
    case num_sub_op: return print(p, '-');
    case num_mul_op: return print(p, '*');
    case num_div_op: return print(p, '/');
    case num_mod_op: return print(p, '%');
    case bit_and_op: return print(p, '&');
    case bit_or_op: return print(p, '|');
    case bit_xor_op: return print(p, '^');
    case bit_lsh_op: return print(p, "<<");
    case bit_rsh_op: return print(p, ">>");
    case rel_eq_op: return print(p, "==");
    case rel_ne_op: return print(p, "!=");
    case rel_lt_op: return print(p, "<");
    case rel_gt_op: return print(p, ">");
    case rel_le_op: return print(p, "<=");
    case rel_ge_op: return print(p, ">=");
    case log_and_op: return print(p, "&&");
    case log_or_op: return print(p, "||");
  }
  lingo_unreachable("invalid binary operator ({})", (int)op);
}


void 
debug(Printer& p, Unary_op op)
{
  print(p, op);
}


void 
debug(Printer& p, Binary_op op)
{
  print(p, op);
}


} // namespace beaker
