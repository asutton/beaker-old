// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/parse.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/stmt.hpp"
#include "beaker/decl.hpp"
#include "beaker/operator.hpp"


namespace beaker
{

Stmt const* parse_file(Parser&, Token_stream&);


Stmt const*
parse(Token_list const& toks)
{
  // Note that the token stream will update the input
  // context as we consume tokens.
  Token_stream ts(toks);
  
  Parser p;
  return parse_file(p, ts);
}


// -------------------------------------------------------------------------- //
//                              Type semantics


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
Parser::on_int_type(Token const*)
{
  return get_int_type();
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
  error(tok->location(), "unresolved symbol '{}'", *tok);
  return make_error_node<Expr>();
}



Expr const*
Parser::on_call_expr(Token const* tok, Expr const* fn, Expr_seq const& args)
{
  return make_call_expr(tok->location(), fn, args);
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
  Location loc = tok->location();
  switch (tok->kind()) {
  case plus_tok: 
    return make_binary_expr(loc, num_add_op, e1, e2);
  case minus_tok: 
    return make_binary_expr(loc, num_sub_op, e1, e2);
  case star_tok: 
    return make_binary_expr(loc, num_mul_op, e1, e2);
  case slash_tok: 
    return make_binary_expr(loc, num_div_op, e1, e2);
  case percent_tok: 
    return make_binary_expr(loc, num_mod_op, e1, e2);
  case eq_eq_tok: 
    return make_binary_expr(loc, rel_eq_op, e1, e2);
  case bang_eq_tok: 
    return make_binary_expr(loc, rel_ne_op, e1, e2);
  case lt_tok: 
    return make_binary_expr(loc, rel_lt_op, e1, e2);
  case gt_tok: 
    return make_binary_expr(loc, rel_gt_op, e1, e2);
  case lt_eq_tok: 
    return make_binary_expr(loc, rel_le_op, e1, e2);
  case gt_eq_tok: 
    return make_binary_expr(loc, rel_ge_op, e1, e2);
  case amp_tok: 
    return make_binary_expr(loc, bit_and_op, e1, e2);
  case caret_tok: 
    return make_binary_expr(loc, bit_xor_op, e1, e2);
  case bar_tok: 
    return make_binary_expr(loc, bit_or_op, e1, e2);
  case lt_lt_tok: 
    return make_binary_expr(loc, bit_lsh_op, e1, e2);
  case gt_gt_tok: 
    return make_binary_expr(loc, bit_rsh_op, e1, e2);
  case amp_amp_tok: 
    return make_binary_expr(loc, log_and_op, e1, e2);
  case bar_bar_tok: 
    return make_binary_expr(loc, log_or_op, e1, e2);
  default:
    break;
  }
  lingo_unreachable();
}


Decl const*
Parser::on_variable_decl(Token const*, Type const*, Expr const*)
{
  return nullptr;
}


Decl const*
Parser::on_function_decl(Token const*, Type const*, Type_seq const&, Stmt const*)
{
  return nullptr;
}


Stmt const*
Parser::on_empty_stmt(Token const*)
{
  return  nullptr;
}


Stmt const*
Parser::on_block_stmt(Token const* l, Token const* r, Stmt_seq const& s)
{
  return make_block_stmt(l->location(), r->location(), s);
}


Stmt const*
Parser::on_declaration_stmt(Decl const*)
{
  return  nullptr;
}


Stmt const*
Parser::on_if_stmt(Token const*, Expr const*, Stmt const*)
{
  return  nullptr;
}


Stmt const*
Parser::on_while_stmt(Token const*, Expr const*, Stmt const*)
{
  return  nullptr;
}


Stmt const*
Parser::on_do_stmt(Token const*, Token const*, Expr const*, Stmt const*)
{
  return  nullptr;
}


Stmt const*
Parser::on_return_stmt(Token const*, Expr const*)
{
  return  nullptr;
}




Stmt const*
Parser::on_expression_stmt(Token const*, Expr const*)
{
  return nullptr;
}


Stmt const*
Parser::on_assignment_stmt(Token const*, Token const*, Expr const*, Expr const*)
{
  return nullptr;
}


// Simply construct a new node comprising those statements.
Stmt const*
Parser::on_file(Stmt_seq const& s)
{
  return make_file_stmt(s);
}


// TODO: Install the grammar rule/name associations.
void
init_grammar()
{
  
}


} // namespace beaker

