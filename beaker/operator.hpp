// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_OPERATOR_HPP
#define BEAKER_OPERATOR_HPP

// This module defines the kinds of unary and binary
// operations supported by the language.

#include "beaker/prelude.hpp"

namespace beaker
{

// Kinds of unary operators.
enum Unary_op
{
  num_neg_op, // -e
  num_pos_op, // +e
  bit_not_op, // ~e
  log_not_op, // !e
};


// Kinds of binary operators.
enum Binary_op
{
  num_add_op, // e1 + e2
  num_sub_op, // e1 - e2
  num_mul_op, // e1 * e2
  num_div_op, // e1 / e2
  num_mod_op, // e1 % e2
  bit_and_op, // e1 & e2
  bit_or_op,  // e1 | e2
  bit_xor_op, // e1 ^ e2
  bit_lsh_op, // e1 << e2
  bit_rsh_op, // e1 >> e2
  rel_eq_op,  // e1 == e2
  rel_ne_op,  // e1 != e2
  rel_lt_op,  // e1 < e2
  rel_gt_op,  // e2 > e2
  rel_le_op,  // e1 <= e2
  rel_ge_op,  // e1 >= e2
  log_and_op, // e1 && e2
  log_or_op,  // e1 || e2
};


Type const* get_type(Unary_op, Expr const*);
Type const* get_type(Binary_op, Expr const*, Expr const*);


char const* get_spelling(Unary_op);
char const* get_spelling(Binary_op);


void print(Printer& p, Unary_op);
void print(Printer& p, Binary_op);


void debug(Printer& p, Unary_op);
void debug(Printer& p, Binary_op);



} // namespace beaker


#endif
