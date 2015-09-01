// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/parser.hpp"
#include "beaker/builder.hpp"
#include "beaker/lookup.hpp"

namespace beaker
{


// The type of a parse function. Use to disambiguate
// overloads in certain cases.
using Parse_fn = Expr const* (*)(Parser&, Token_stream&);


// -------------------------------------------------------------------------- //
//                                Type parsers

Type const* parse_type(Parser&, Token_stream&);


// Parse a type.
//
//    type ::= 'void' | 'bool' | 'int'
//
// TODO: Add a parser for function and tuple types.
Type const*
parse_type(Parser& p, Token_stream& ts)
{
  switch (next_token_kind(ts)) {
    case void_kw:
      return p.on_void_type(get_token(ts));
    case bool_kw:
      return p.on_bool_type(get_token(ts));
    case int_kw:
      return p.on_intr_type(get_token(ts));
    default:
      break;
  }
  
  error(ts.location(), "invalid type '{}'", ts.peek());
  return make_error_node<Type>();
}


// -------------------------------------------------------------------------- //
//                            Expression parsers
//


Expr const* parse_primary_expr(Parser&, Token_stream&);
Expr const* parse_postfix_expr(Parser&, Token_stream&);
Expr const* parse_prefix_expr(Parser&, Token_stream&);
Expr const* parse_binary_expr(Parser&, Token_stream&);
Expr const* parse_expr(Parser&, Token_stream&);


// -------------------------------------------------------------------------- //
//                            Primary expressions


// Parse a nested sub-expression.
//
//    nested-expression ::= '(' expression ')'
Expr const*
parse_nested_expr(Parser& p, Token_stream& ts)
{
  // TODO: Improve diagnostics.
  if (Required<Enclosed_term<Expr>> e = parse_paren_enclosed(p, ts, parse_expr))
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
  error(ts.location(), "invalid primary-expression");
  return get_error_expr();
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
//
// TODO: This is copying the argument vector. We should be
// moving it.
Expr const*
parse_call_expr(Parser& p, Token_stream& ts, Expr const* expr) 
{
  using Arg_list = Enclosed_term<Arg_seq>;

  Token const* tok = ts.begin(); 
  if (Required<Arg_list> args = parse_paren_enclosed(p, ts, parse_argument_list))
    return p.on_call_expr(tok, expr, args->term());
  else
    return get_error_expr();
}


// Parse a postfix expression. This is the entry point to all
// binary or n-ary expressions parsed at this precedence.
//
//    postfix-expression ::= call-expression
//                         | primary-expression
//
// TODO: Add subscript expressions. Other stuff?
Expr const*
parse_postfix_expr(Parser& p, Token_stream& ts) {
  if (Expr const* e1 = parse_primary_expr(p, ts)) {
    while (e1) {
      Expr const* e2;
      switch (next_token_kind(ts)) {
      case lparen_tok: 
        e2 = parse_call_expr(p, ts, e1);
        break;

      case dot_tok:
        e2 = parse_member_expr(p, ts, e1);
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
//                           Prefix expressions

// Parse a prefix operator.
//
//    prefix-operator ::= '+' | '-' | '!'
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
  auto act = [&](Token const* tok, Expr const* e)
  {
    return p.on_unary_expr(tok, e);
  };

  return parse_prefix_term(p, ts, op, sub, act);
}


Expr const*
parse_expr(Parser& p, Token_stream& ts)
{
  return parse_postfix_expr(p, ts);
}



String const* 
Parser::on_name(Token const* tok)
{
  return tok->str();
}


// -------------------------------------------------------------------------- //
//                              Type semantics


Type const*
Parser::on_id_type(Token const* tok)
{
  Decl const* decl = lookup_decl(tok->str());
  if (!decl)
    return get_error_type();

  // TODO: Support type aliases.
  return get_user_defined_type(decl);
}


Type const*
Parser::on_void_type(Token const*)
{
  return get_void_type();
}


Type const*
Parser::on_bool_type(Token const*)
{
  return get_bool_type();
}


Type const*
Parser::on_int_type(Token const*, int p, Integer_sign s, Integer_order o)
{
  return get_int_type(p, s, o);
}


// -------------------------------------------------------------------------- //
//                            Expression semantics

Expr const*
Parser::on_boolean_lit(Token const* tok)
{
  return make_bool_expr(tok->location(), as_bool(*tok));
}


Expr const*
Parser::on_integer_lit(Token const* tok)
{
  return make_int_expr(tok->location(), as_int(*tok));
}


Expr const*
Parser::on_identifier_expr(Token const* tok)
{
  // FIXME: Implement me.
  // if (Required<Decl> decl = lookup(tok->str()))
  //   return make_identifier_expr(tok->location(), decl->term());
  error("unresolved symbol '{}'", tok);
  return make_error_node<Expr>();
}



Expr const*
Parser::on_call_expr(Token const* tok, Expr const* fn, Arg_seq const* args)
{
  return make_call_expr(tok->location(), fn, *args);
}


Expr const*
Parser::on_unary_expr(Token const* tok, Expr const* e)
{
  Location loc = tok->location();
  switch (tok->kind()) {
  case plus_tok: 
    return make_unary_expr(loc, num_pos_op, e);
  case minus_tok: 
    return make_unary_expr(loc, num_neg_op, e);
  case tilde_tok: 
    return make_unary_expr(loc, bit_not_op, e);
  case bang_tok: 
    return make_unary_expr(loc, log_not_op, e);
  default:
    break;
  }
  lingo_unreachable();
}


Expr const*
Parser::on_binary_expr(Token const* tok, Expr const* e1, Expr const* e2)
{
  
}


// TODO: Install the grammar rule/name associations.
void
init_grammar()
{
  
}


} // namespace beaker

