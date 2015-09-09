// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/parse.hpp"
#include "lookup.hpp"

namespace beaker
{


Type const* parse_type(Parser&, Token_stream&);
Expr const* parse_expr(Parser&, Token_stream&);
Decl const* parse_decl(Parser&, Token_stream&);
Stmt const* parse_stmt(Parser&, Token_stream&);


namespace
{


// Parse a type clause.
//
//    type-clause ::= ':' type
Type const*
parse_type_clause(Parser& p, Token_stream& ts)
{
  if (expect_token(p, ts, colon_tok))
    if (Required<Type> type = parse_expected(p, ts, parse_type))
      return *type;
  return make_error_node<Type>();
}


// Parse an initializer clause.
//
//    initializer-clause ::= '=' expr
Expr const*
parse_initializer_clause(Parser& p, Token_stream& ts)
{
  if (expect_token(p, ts, eq_tok))
    if (Required<Expr> init = parse_expected(p, ts, parse_expr))
      return *init;
  return make_error_node<Expr>();
}


// Parse a variable declaration.
//
//    variable-decl ::= 'var' identifier type-clause initializer-clause;'
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
    return make_error_node<Decl>();

  // Match the type clause.
  Required<Type> type = parse_type_clause(p, ts);
  if (!type)
    return make_error_node<Decl>();

  // Point of declaration. Having a name and type, we can
  // partially form the declaration.
  //
  // TODO: If this fails, consume up to the next ';' and
  // resume parsing.
  Required<Decl> var = p.on_variable_decl(tok, id, *type);
  if (!var)
    return make_error_node<Decl>();

  // Match the initializer clause.
  Required<Expr> init = parse_initializer_clause(p, ts);
  if (!init)
    return make_error_node<Decl>();

  // Check for the semicolon, but allow its omission.
  expect_token(p, ts, semicolon_tok);

  return p.on_variable_init(*var, *init);
}


using Parm_seq = Sequence_term<Decl>;
using Parm_clause = Enclosed_term<Parm_seq>;


// Parse a parameter declaration.
//
//    parameter-decl ::= identifier type-clause
//
// TODO: Support default function arguments?
Decl const*
parse_parameter_decl(Parser& p, Token_stream& ts)
{
  if (Token const* id = expect_token(p, ts, identifier_tok))
    if (Required<Type> type = parse_type_clause(p, ts))
      return p.on_parameter_decl(id, *type);
  return make_error_node<Decl>();
}


// Parse a comma-separated list of parameter declarations.
//
//    parameter-list ::= parameter-decl [',' parameter-decl]*
Parm_seq const*
parse_parameter_list(Parser& p, Token_stream& ts)
{
  return parse_comma_list(p, ts, parse_parameter_decl);
}


// Parse a parameter declaration clause.
//
//    parameter-clause ::= '()' | '(' parameter-list ')'
Parm_clause const*
parse_parameter_clause(Parser& p, Token_stream& ts)
{
  return parse_paren_enclosed(p, ts, parse_parameter_list);
}


// Parse a return clause.
//
//    return-clause ::= '->' type
Type const*
parse_return_clause(Parser& p, Token_stream& ts)
{
  if (expect_token(p, ts, arrow_tok))
    if (Required<Type> type = parse_expected(p, ts, parse_type))
      return *type;
  return make_error_node<Type>();
}


// Parse a function definition.
//
//    function-def ::= block-stmt
//                   | '=' expression-stmt
//
// TODO: Actually parse a block or expression statement.
Stmt const*
parse_function_def(Parser& p, Token_stream& ts)
{
  // Match but don't consume the '{'.
  if (next_token_is(ts, lbrace_tok))
    return parse_stmt(p, ts);
  
  // Match and consume the '=' token, so that we parse
  // the full expression statement (including the ';').
  if (match_token(ts, eq_tok))
    return parse_stmt(p, ts);
  
  error(ts.location(), "expected function-definition, but got {}", ts.peek());
  return make_error_node<Stmt>();
}


// Parse a function declaration.
//
//    function-decl ::= 'def' identifier parameter-clause '->' type stmt
//
//    parameter-clause ::= '()' | (' parameter-seq ')'
//
//
// Note that the stmt must be a block statement.
//
// TODO: Allow function redeclaration? Possibly with a different
// syntax:
Decl const*
parse_function_decl(Parser& p, Token_stream& ts)
{
  Token const* tok = require_token(ts, def_kw);

  // match the identifier.
  Token const* id = expect_token(p, ts, identifier_tok);
  if (!id)
    return make_error_node<Decl>();

  // Parse the parameter declaration clause.
  Required<Parm_clause> parms = parse_parameter_clause(p, ts);
  if (!parms)
    return make_error_node<Decl>();

  // Match the type clause.
  Required<Type> type = parse_return_clause(p, ts);
  if (!type)
    return make_error_node<Decl>();

  // Point of declaration. Having a name, parameters, and return
  // type, we can declare the function.
  Required<Decl> fn = p.on_function_decl(tok, id, *parms->term(), *type);
  if (!fn)
    return make_error_node<Decl>();

  // Enter the function scope. 
  Function_scope scope;

  // Start the function, causing the parameters to be declared.
  fn = p.on_function_start(*fn);
  if (!fn)
    return make_error_node<Decl>();

  // Parse the function definition.
  // TODO: We should probably immediately parse a com
  Required<Stmt> body = parse_expected(p, ts, parse_stmt);

  return p.on_function_finish(*fn, *body);
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
