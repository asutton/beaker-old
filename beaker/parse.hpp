// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_PARSER_HPP
#define BEAKER_PARSER_HPP

#include "beaker/prelude.hpp"
#include "beaker/token.hpp"

#include "lingo/parsing.hpp"


namespace beaker
{


// ---------------------------------------------------------------------------//
//                          Parsing interface


// The parser is responsible for constructing syntax nodes
// from information provided by parse functions.
//
// Note that the members of this class expose certain hooks in
// the grammar.
struct Parser
{
  using result_type = void*;

  String const* on_name(Token const*);

  Type const* on_void_type(Token const*);
  Type const* on_bool_type(Token const*);
  Type const* on_int_type(Token const*);

  Expr const* on_boolean_lit(Token const*);
  Expr const* on_integer_lit(Token const*);
  Expr const* on_identifier_expr(Token const*);
  Expr const* on_integer_expr(Token const*);
  Expr const* on_member_expr(Token const*, Expr const*, Expr const*);
  Expr const* on_unary_expr(Token const*, Expr const*);
  Expr const* on_binary_expr(Token const*, Expr const*, Expr const*);
  Expr const* on_call_expr(Token const*, Expr const*, Expr_seq const&);

  Decl const* on_variable_decl(Token const*, Token const*, Type const*);
  Decl const* on_variable_init(Decl const*, Expr const*);
  Decl const* on_function_decl(Token const*, Token const*, Decl_seq const&, Type const*);
  Decl const* on_function_start(Decl const*);
  Decl const* on_function_finish(Decl const*, Stmt const*);
  Decl const* on_parameter_decl(Token const*, Type const*);

  Stmt const* on_empty_stmt(Token const*);
  Stmt const* on_block_stmt(Token const*, Token const*, Stmt_seq const&);
  Stmt const* on_declaration_stmt(Decl const*);
  Stmt const* on_if_stmt(Token const*, Expr const*, Stmt const*);
  Stmt const* on_while_stmt(Token const*, Expr const*, Stmt const*);
  Stmt const* on_do_stmt(Token const*, Token const*, Expr const*, Stmt const*);
  Stmt const* on_return_stmt(Token const*, Token const*, Expr const*);
  Stmt const* on_expression_stmt(Token const*, Expr const*);
  Stmt const* on_assignment_stmt(Token const*, Token const*, Expr const*, Expr const*);

  Stmt const* on_file(Stmt_seq const&);
};


void init_grammar();

Stmt const* parse(Token_list const&);


// ---------------------------------------------------------------------------//
//                          Parsing support

// Parse a paren-enclosed term.
template<typename Parser, 
         typename Stream, 
         typename Rule,
         typename Term = Term_type<Parser, Stream, Rule>>
Enclosed_term<Term> const*
parse_paren_enclosed(Parser& p, Stream& ts, Rule rule)
{
  return parse_enclosed(p, ts, lparen_tok, rparen_tok, rule);
}


// Parse a brace-enclose term.
template<typename Parser, 
         typename Stream, 
         typename Rule,
         typename Term = Term_type<Parser, Stream, Rule>>
Enclosed_term<Term> const*
parse_brace_enclosed(Parser& p, Stream& ts, Rule rule)
{
  return parse_enclosed(p, ts, lbrace_tok, rbrace_tok, rule);
}


// Parse a comma-separated list of terms.
template<typename Parser, 
         typename Stream, 
         typename Rule,
         typename Term = Term_type<Parser, Stream, Rule>>
inline Sequence_term<Term> const*
parse_comma_list(Parser& p, Stream& ts, Rule rule)
{
  return parse_list(p, ts, comma_tok, rule);
}

} // namespace beaker


#endif
