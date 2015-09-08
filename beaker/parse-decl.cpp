// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/parse.hpp"


namespace beaker
{


Type const* parse_type(Parser&, Token_stream&);
Expr const* parse_expr(Parser&, Token_stream&);
Decl const* parse_decl(Parser&, Token_stream&);
Stmt const* parse_stmt(Parser&, Token_stream&);


namespace
{


// Used to propagate diagnosed errors.
Decl const* error_decl = make_error_node<Decl>();



// Parse a variable declaration.
//
//    variable-decl ::= 'var' identifier ':' type '=' expr ';'
//
// TODO: Support alternative forms of variable declaration:
//
//    'var' identifier ':' type ';' # Default construction
//    'var' identifier '=' expr ';' # Type deduction
Decl const*
parse_variable_decl(Parser& p, Token_stream& ts)
{
  Token const* tok = require_token(ts, var_kw);

  // match the identifier.
  Token const* id = expect_token(p, ts, identifier_tok);
  if (!id)
    return error_decl;

  // Match the type clause.
  if (!expect_token(p, ts, colon_tok))
    return error_decl;
  Required<Type> type = parse_expected(p, ts, parse_type);
  if (!type)
    return error_decl;

  // Point of declration. Having a name and type, we can
  // partially form the declaration.
  //
  // TODO: If this fails, consume up to the next ';' and
  // resume parsing.
  Required<Decl> var = p.on_variable_decl(tok, id, *type);
  if (!var)
    return error_decl;

  // Match the initializer clause.
  if (!expect_token(p, ts, eq_tok))
    return error_decl;
  Required<Expr> init = parse_expected(p, ts, parse_expr);

  // Check for the semicolon, but allow its omission.
  expect_token(p, ts, semicolon_tok);

  return p.on_variable_init(*var, *init);
}


// Parse a function declaration.
//
//    function-decl ::= 'def' identifier parameter-clause '->' type stmt
//
//    parameter-clause ::= '()' | (' parameter-seq ')'
//
//    parameter-seq ::= parameter-decl [',' parameter-decl]*
//
// Note that the stmt must be a block statement.
//
// TODO: Allow function redeclaration? Possibly with a different
// syntax:
Decl const*
parse_function_decl(Parser& p, Token_stream& ts)
{
  Token const* tok = require_token(ts, def_kw);
  error(tok->location(), "not implemented");
  return error_decl;
}


} // namespace


// Parse a declaration.
//
//    decl ::= variable-decl
//           | function-decl
Decl const*
parse_decl(Parser& p, Token_stream& ts)
{
  switch (next_token_kind(ts)) {
    case var_kw: return parse_variable_decl(p, ts);
    case def_kw: return parse_function_decl(p, ts);
    default: break;
  }

  // We can't get here because we know that the current token
  // is one of those above.
  lingo_unreachable();
}


} // namepace beaker
