// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/parse.hpp"


namespace beaker
{


Expr const* parse_expr(Parser&, Token_stream&);
Decl const* parse_decl(Parser&, Token_stream&);
Stmt const* parse_stmt(Parser&, Token_stream&);


namespace
{


// Used to propagate diagnosed errors.
Stmt const* error_stmt = make_error_node<Stmt>();


// Parse an declaration statement.
//
//    declaration-stmt ::= decl
Stmt const*
parse_declaration_stmt(Parser& p, Token_stream& ts)
{
  if (Required<Decl> d = parse_decl(p, ts))
    return p.on_declaration_stmt(*d);
  return error_stmt;
}


// Parse a expression statement.
//
//    expression-stmt ::= expr ';' 
//                      | expr '=' expr ';'
Stmt const*
parse_expression_stmt(Parser& p, Token_stream& ts)
{
  Required<Expr> e1 = parse_expected(p, ts, parse_expr);
  if (!e1)
    return error_stmt;

  // Match the optional assignment form.
  Required<Expr> e2;
  Token const* assign = match_token(ts, eq_tok);
  if (assign) {
    e2 = parse_expected(p, ts, parse_expr);
    if (!e2)
      return error_stmt;
  }

  // Diagnose a missing semicolon, but continue processing.
  Token const* semi = match_token(ts, semicolon_tok);
  if (!semi)
    error(ts.location(), "missing ';'");
  
  // Handle the result of the parse.
  if (assign)  
    return p.on_assignment_stmt(assign, semi, *e1, *e2);
  else
    return p.on_expression_stmt(semi, *e1);
}


} // namespace


// Parse a statement.
//
//    statement ::= declaration-statement
//                | block-statement
//                | return-statement
//                | expression-statement
Stmt const*
parse_stmt(Parser& p, Token_stream& ts)
{
  switch (next_token_kind(ts)) {
    case var_kw:
    case def_kw:
      return parse_declaration_stmt(p, ts);
    
    // TOOD: Parse other statements!

    default:
      return parse_expression_stmt(p, ts);
  }
  
  error(ts.location(), "invalid statement '{}'", ts.peek());
  return make_error_node<Stmt>();
}


} // namepace beaker
