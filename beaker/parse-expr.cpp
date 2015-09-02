// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/parse.hpp"


namespace beaker
{

Expr const* parse_expr(Parser&, Token_stream&);


namespace
{

Expr const* parse_primary_expr(Parser&, Token_stream&);
Expr const* parse_postfix_expr(Parser&, Token_stream&);
Expr const* parse_prefix_expr(Parser&, Token_stream&);
Expr const* parse_binary_expr(Parser&, Token_stream&);


// -------------------------------------------------------------------------- //
//                            Primary expressions


// Parse a nested sub-expression.
//
//    nested-expression ::= '(' expression ')'
Expr const*
parse_nested_expr(Parser& p, Token_stream& ts)
{
  // TODO: Improve diagnostics.
  if (Required<Enclosed_expr> e = parse_paren_enclosed(p, ts, parse_expr))
    return e->term();
  return make_error_node<Expr>();
}


// Parse a primary expression.
//
//    primary-expression ::= literal 
//                         | identifier-expression 
//                         | nested-expression
//
//    literal ::= boolean-literal | integer-literal
Expr const*
parse_primary_expr(Parser& p, Token_stream& ts)
{
  switch (next_token_kind(ts)) {
    case boolean_tok:
      return p.on_boolean_lit(get_token(ts));
    
    case integer_tok: 
      return p.on_integer_lit(get_token(ts));
    
    case identifier_tok:
      return p.on_identifier_expr(get_token(ts));
    
    case lparen_tok:
      return parse_nested_expr(p, ts);

    default:
      break;
  }

  // Don't report an error. Let a lower-precedence parse
  // report the problem instead of the highest.
  return nullptr;
}


// -------------------------------------------------------------------------- //
//                            Postfix expressions


// Parse a function argument.
//
//    function-argument ::= expression
//
// This is a placeholder that would allow us to accept
// non-expression arguments in the future.
inline Expr const*
parse_argument(Parser& p, Token_stream& ts)
{
  return parse_expr(p, ts);
}


// Parse an argument list.
//
//    function-argument-list ::= list(function-argument)
inline Arg_seq const*
parse_argument_list(Parser& p, Token_stream& ts)
{
  return parse_list(p, ts, comma_tok, parse_argument);
}


// Parse a call expression.
//
//    call-expression ::= postfix-expression '(' argument-list ')'
Expr const*
parse_call_expr(Parser& p, Token_stream& ts, Expr const* expr) 
{
  Token const* tok = ts.begin(); 
  if (Required<Enclosed_args> args = parse_paren_enclosed(p, ts, parse_argument_list))
    return p.on_call_expr(tok, expr, args->term());
  else
    return make_error_node<Expr>();
}


// Parse a postfix expression. This is the entry point to all
// binary or n-ary expressions parsed at this precedence.
//
//    postfix-expression ::= call-expression
//                         | primary-expression
Expr const*
parse_postfix_expr(Parser& p, Token_stream& ts) {
  if (Expr const* e1 = parse_primary_expr(p, ts)) {
    while (e1) {
      Expr const* e2;
      switch (next_token_kind(ts)) {
      case lparen_tok: 
        e2 = parse_call_expr(p, ts, e1);
        break;

      default:
        e2 = nullptr;
        break;
      }

      if (not e2)
        break;

      e1 = e2;
    }
    return e1;
  }
  return {};
}


// -------------------------------------------------------------------------- //
//                           Unary expressions


// A helper function object for dispatching to the
// semantic action for unary expressions.
struct on_unary_expr
{
  on_unary_expr(Parser& p)
    : p(p)
  { }

  Expr const* operator()(Token const* tok, Expr const* e)
  {
    return p.on_unary_expr(tok, e);
  }

  Parser& p;
};


// Parse a unary operator.
//
//    unary-operator ::= '+' | '-' | '!'
Token const*
parse_unary_op(Parser& p, Token_stream& ts)
{
  switch (next_token_kind(ts)) {
  case plus_tok:
  case minus_tok:
  case bang_tok:
  case tilde_tok:
    return get_token(ts);

  default:
    return nullptr;
  }
}


// Parse a unary expression.
//
//    unary-expression ::= postfix-expression
//                       | unary-operator unary-expression
inline Expr const*
parse_unary_expr(Parser& p, Token_stream& ts)
{
  auto op = parse_unary_op;
  auto sub = parse_postfix_expr;
  return parse_prefix_term(p, ts, op, sub, on_unary_expr(p));
}



// -------------------------------------------------------------------------- //
//                        Binary expressions


// A helper function object for dispatching to the
// semantic action for binary expressions.
struct on_binary_expr
{
  on_binary_expr(Parser& p)
    : p(p)
  { }

  Expr const* operator()(Token const* tok, Expr const* e1, Expr const* e2)
  {
    return p.on_binary_expr(tok, e1, e2);
  }

  Parser& p;
};


// Parse a multiplicative expression.
//
//    multiplicative-expression ::= 
//        unary-expression
//      | multiplicative-expression '*' unary-expression
//      | multiplicative-expression '/' unary-expression
//      | multiplicative-expression '%' unary-expression

// Match one of the multiplicative operators.
inline Token const*
parse_multiplicative_op(Parser& p, Token_stream& ts)
{
  switch (next_token_kind(ts)) {
    case star_tok:
    case slash_tok:
    case percent_tok:
      return get_token(ts);
    default:
      break;
  }
  return nullptr;
}


// Parse a multiplicative expression.
Expr const*
parse_multiplicative_expr(Parser& p, Token_stream& ts)
{
  auto op = parse_multiplicative_op;
  auto sub = parse_unary_expr;
  return parse_left_infix_term(p, ts, op, sub, on_binary_expr(p));
}


// Parse an additive expression.
//
//    additive-expression ::= 
//        multiplicative-expression
//      | additive-expression '+' multiplicative-expression
//      | additive-expression '-' multiplicative-expression

// Match one of the additive operators.
inline Token const*
parse_additive_op(Parser& p, Token_stream& ts)
{
  switch (next_token_kind(ts)) {
    case plus_tok:
    case minus_tok:
      return get_token(ts);
    default:
      break;
  }
  return nullptr;
}


// Parse an additive expression.
Expr const*
parse_additive_expr(Parser& p, Token_stream& ts)
{
  auto op = parse_additive_op;
  auto sub = parse_multiplicative_expr;
  return parse_left_infix_term(p, ts, op, sub, on_binary_expr(p));
}


// Parse a shift expression.
//
//    shift-expression ::= 
//        additive-expression
//      | shift-expression '<<' additive-expression
//      | shift-expression '>>' additive-expression

// Match one of the shift operators.
inline Token const*
parse_shift_op(Parser& p, Token_stream& ts)
{
  switch (next_token_kind(ts)) {
    case lt_lt_tok:
    case gt_gt_tok:
      return get_token(ts);
    default:
      break;
  }
  return nullptr;
}


// Parse a shift expression.
Expr const*
parse_shift_expr(Parser& p, Token_stream& ts)
{
  auto op = parse_shift_op;
  auto sub = parse_additive_expr;
  return parse_left_infix_term(p, ts, op, sub, on_binary_expr(p));
}


// Parse a relational expression.
//
//    relational-expression ::= 
//        shift-expression
//      | relational-expression '<' shift-expression
//      | relational-expression '>' shift-expression
//      | relational-expression '<=' shift-expression
//      | relational-expression '>=' shift-expression

// Match one of the relational operators.
inline Token const*
parse_relational_op(Parser& p, Token_stream& ts)
{
  switch (next_token_kind(ts)) {
    case lt_tok:
    case gt_tok:
    case lt_eq_tok:
    case gt_eq_tok:
      return get_token(ts);
    default:
      break;
  }
  return nullptr;
}


// Parse a relational expression.
Expr const*
parse_relational_expr(Parser& p, Token_stream& ts)
{
  auto op = parse_relational_op;
  auto sub = parse_shift_expr;
  return parse_left_infix_term(p, ts, op, sub, on_binary_expr(p));
}


// Parse an equality expression.
//
//    equality-expression ::= 
//        relational-expression
//      | equality-expression '==' relational-expression
//      | equality-expression '!=' relational-expression

// Match one of the equality operators.
inline Token const*
parse_equality_op(Parser& p, Token_stream& ts)
{
  switch (next_token_kind(ts)) {
    case eq_eq_tok:
    case bang_eq_tok:
      return get_token(ts);
    default:
      break;
  }
  return nullptr;
}


// Parse a equality expression.
Expr const*
parse_equality_expr(Parser& p, Token_stream& ts)
{
  auto op = parse_equality_op;
  auto sub = parse_relational_expr;
  return parse_left_infix_term(p, ts, op, sub, on_binary_expr(p));
}


// Parse a bitwise and expression.
//
//    bit-and-expression ::= 
//        equality-expression
//      | bit-and-expression '&' equality-expression

// Match the bitwise and operator.
inline Token const*
parse_bit_and_op(Parser& p, Token_stream& ts)
{
  return match_token(ts, amp_tok);
}


// Parse a bitwise and expression.
Expr const*
parse_bit_and_expr(Parser& p, Token_stream& ts)
{
  auto op = parse_bit_and_op;
  auto sub = parse_equality_expr;
  return parse_left_infix_term(p, ts, op, sub, on_binary_expr(p));
}


// Parse a bitwise exclusive or expression.
//
//    bit-xor-expression ::= 
//        bitwise-and-expression
//      | bit-xor-expression '^' bitwise-and-expression

// Match the bitwise xor operator.
inline Token const*
parse_bit_xor_op(Parser& p, Token_stream& ts)
{
  return match_token(ts, caret_tok);
}


// Parse a bitwise xor expression.
Expr const*
parse_bit_xor_expr(Parser& p, Token_stream& ts)
{
  auto op = parse_bit_xor_op;
  auto sub = parse_bit_and_expr;
  return parse_left_infix_term(p, ts, op, sub, on_binary_expr(p));
}


// Parse a bitwise inclusive or expression.
//
//    bit-or-expression ::= 
//        bitwise-xor-expression
//      | bit-or-expression '|' bitwise-xor-expression

// Match the bitwise or operator.
inline Token const*
parse_bit_or_op(Parser& p, Token_stream& ts)
{
  return match_token(ts, bar_tok);
}


// Parse a bitwise or expression.
Expr const*
parse_bit_or_expr(Parser& p, Token_stream& ts)
{
  auto op = parse_bit_or_op;
  auto sub = parse_bit_xor_expr;
  return parse_left_infix_term(p, ts, op, sub, on_binary_expr(p));
}


// Parse a logical and expression.
//
//    logical-and-expression ::= 
//        bitwise-or-expression
//      | logical-and-expression '&&' bitwise-or-expression

// Match the logical and operator.
inline Token const*
parse_lgoical_and_op(Parser& p, Token_stream& ts)
{
  return match_token(ts, bar_tok);
}


// Parse a logical and expression.
Expr const*
parse_logical_and_expr(Parser& p, Token_stream& ts)
{
  auto op = parse_lgoical_and_op;
  auto sub = parse_bit_or_expr;
  return parse_left_infix_term(p, ts, op, sub, on_binary_expr(p));
}


// Parse a logical or expression.
//
//    logical-or-expression ::= 
//        logical-and-expression
//      | logical-or-expression '||' logical-and-expression


// Match the logical or operator.
inline Token const*
parse_lgoical_or_op(Parser& p, Token_stream& ts)
{
  return match_token(ts, bar_tok);
}


// Parse a logical or expression.
Expr const*
parse_logical_or_expr(Parser& p, Token_stream& ts)
{
  auto op = parse_lgoical_or_op;
  auto sub = parse_logical_and_expr;
  return parse_left_infix_term(p, ts, op, sub, on_binary_expr(p));
}


inline Expr const*
parse_binary_expr(Parser& p, Token_stream& ts)
{
  return parse_logical_or_expr(p, ts);
}


} // namespace


// Parse an expression.
Expr const*
parse_expr(Parser& p, Token_stream& ts)
{
  return parse_binary_expr(p, ts);
}


} // namepace beaker
