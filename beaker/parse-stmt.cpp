// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/parse.hpp"
#include "beaker/lookup.hpp"

namespace beaker
{


Expr const* parse_expr(Parser&, Token_stream&);
Decl const* parse_decl(Parser&, Token_stream&);
Stmt const* parse_stmt(Parser&, Token_stream&);


namespace
{


// Consume through the next semicolon. This is typically used
// to advance the parse state after a fatal error in an
// expression statement.
//
// TODO: Move the generic form into lingo.
void
consume_through_semicolon(Token_stream& ts)
{
  while (next_token_is_not(ts, semicolon_tok))
    get_token(ts);
}


// Used to propagate diagnosed errors.

// Parse the empty statement.
//
//    empty-stmt ::= ';'
Stmt const*
parse_empty_stmt(Parser& p, Token_stream& ts)
{
  Token const* tok = require_token(ts, semicolon_tok);
  return p.on_empty_stmt(tok);
}


// Parse a sequence of statements.
//
//    statement-seq ::= stmt [',' stmt*]
Sequence_term<Stmt> const*
parse_statement_seq(Parser& p, Token_stream& ts)
{
  using Result = Sequence_term<Stmt>;
  Result result;
  while (!ts.eof()) {
    if (next_token_is(ts, rbrace_tok))
      break;

    if (Required<Stmt> term = parse_stmt(p, ts))
      result.push_back(*term);
    
    // Continue parsing even though we got an error, continue
    // parsing. This will allow us to diagnose as many errors
    // as possible in a block statement.
    else if (term.is_error())
      continue;
    
    // We should probably never have this state.
    else if (term.is_empty())
      continue;
  }
  return Result::make(std::move(result));
}


// Parse a block statement.
//
//    block-stmt ::= '{' stmt-seq '}'
//
// Each block defines a new local scope.
Stmt const*
parse_block_stmt(Parser& p, Token_stream& ts)
{
  Local_scope scope; // Enter a new local scope.

  using Term = Enclosed_term<Sequence_term<Stmt>>;
  if (Required<Term> enc = parse_brace_enclosed(p, ts, parse_statement_seq)) {
    // Note that enc->term() may be empty (null) if there are
    // no statements in the block.
    if (enc->term())
      return p.on_block_stmt(enc->open(), enc->close(), *enc->term());
    else
      return p.on_block_stmt(enc->open(), enc->close(), {});
  }
  else
    return make_error_node<Stmt>();
}


// Parse an declaration statement.
//
//    declaration-stmt ::= decl
Stmt const*
parse_declaration_stmt(Parser& p, Token_stream& ts)
{
  if (Required<Decl> d = parse_decl(p, ts))
    return p.on_declaration_stmt(*d);
  return make_error_node<Stmt>();
}


// Represents an expression wrapped in parens.
using Paren_expr = Enclosed_term<Expr>;


// Parse an if-then or if-else statement.
//
//    if-stmt ::= if '(' expr ')' stmt
//              | if '(' expr ')' stmt 'else' stmt
//
Stmt const*
parse_if_stmt(Parser& p, Token_stream& ts)
{
  Token const* tok1 = require_token(ts, if_kw);

  // Match the condition.
  Required<Paren_expr> test = parse_paren_enclosed(p, ts, parse_expr);
  if (!test)
    return make_error_node<Stmt>();
  Expr const* e = test->term();

  // Match the "then" clause.
  Required<Stmt> b1 = parse_expected(p, ts, parse_stmt);
  if (!b1)
    return make_error_node<Stmt>();

  // Match "else stmt".
  if (Token const* tok2 = match_token(ts, else_kw)) {
    Required<Stmt> b2 = parse_expected(p, ts, parse_stmt);
    if (!b2)
      return make_error_node<Stmt>();
    return p.on_if_else_stmt(tok1, tok2, e, *b1, *b2);
  } else {
    return p.on_if_then_stmt(tok1, e, *b1);
  }
}


// Parse a while statement.
//
//    while-stmt ::= 'while' '(' expr ')' stmt
//
// TODO: Factor out the 'while '(' expr ')' as a separate
// clause?
Stmt const*
parse_while_stmt(Parser& p, Token_stream& ts)
{
  error(ts.location(), "not implemented");
  return make_error_node<Stmt>();
}


// Parse a do statement.
//
//    do-stmt ::= 'do' stmt 'while' '(' expr ')' ';'
//
// TODO: See comments above.
Stmt const*
parse_do_stmt(Parser& p, Token_stream& ts)
{
  error(ts.location(), "not implemented");
  return make_error_node<Stmt>();
}


// Parse a return statement.
//
//    return-stmt ::= 'return' [expr] ';'
//
// Note that a return statement with no value is modeled as
// an "exit" statement.
Stmt const*
parse_return_stmt(Parser& p, Token_stream& ts)
{
  Token const* tok = require_token(ts, return_kw);

  // Match the "return;" case.
  if (Token const* semi = match_token(ts, semicolon_tok))
    return p.on_return_stmt(tok, semi);

  if (Required<Expr> expr = parse_expected(p, ts, parse_expr)) {
    // Require the semicolon token, but continue parsing if
    // it's missing.
    Token const* semi = expect_token(p, ts, semicolon_tok);
    if (semi)
      return p.on_return_stmt(tok, semi, *expr);
  }

  // Consume through the next semicolon so we can continue
  // parsing.
  consume_through_semicolon(ts);
  return make_error_node<Stmt>();
}


// Parse a expression statement.
//
//    expression-stmt ::= expr ';' 
//                      | expr '=' expr ';'
//
// An assignment statement is grammatically considered
// to be a form of expression statement. However,
// assignment is not an expression.
Stmt const*
parse_expression_stmt(Parser& p, Token_stream& ts)
{
  Required<Expr> e1 = parse_expected(p, ts, parse_expr);
  if (!e1) {
    consume_through_semicolon(ts);
    return make_error_node<Stmt>();
  }

  // Match the optional assignment form.
  Required<Expr> e2;
  Token const* assign = match_token(ts, eq_tok);
  if (assign) {
    e2 = parse_expected(p, ts, parse_expr);
    if (!e2) {
      consume_through_semicolon(ts);
      return make_error_node<Stmt>();
    }
  }

  // Diagnose a missing semicolon, but continue processing.
  //
  // FIXME: I'm potentially passing a nullptr into the semantic
  // action. Maybe if we're missing the token, I should just
  // insert one at this point?
  //
  // See return-stmt for the same issue.
  Token const* semi = expect_token(p, ts, semicolon_tok);
  
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
    case semicolon_tok:
      return parse_empty_stmt(p, ts);

    case lbrace_tok:
      return parse_block_stmt(p, ts);
    
    case var_kw:
    case def_kw:
      return parse_declaration_stmt(p, ts);

    case if_kw:
      return parse_if_stmt(p, ts);

    case while_kw:
      return parse_while_stmt(p, ts);

    case do_kw:
      return parse_do_stmt(p, ts);

    case return_kw:
      return parse_return_stmt(p, ts);
    
    default:
      return parse_expression_stmt(p, ts);
  }
  lingo_unreachable();
}



} // namepace beaker
