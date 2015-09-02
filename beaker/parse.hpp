// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_PARSER_HPP
#define BEAKER_PARSER_HPP

#include "beaker/prelude.hpp"
#include "beaker/token.hpp"

#include "lingo/parsing.hpp"


namespace beaker
{

// A sequence of function arguments.
using Arg_seq = Sequence_term<Expr>;


// A sequence of function arguments.
using Enclosed_args = Enclosed_term<Arg_seq>;


// An enclosed expression.
using Enclosed_expr = Enclosed_term<Expr>;


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
  Expr const* on_call_expr(Token const*, Expr const*, Arg_seq const*);

  Decl const* on_variable_decl(Token const*, Type const*, Expr const*);
  Decl const* on_function_decl(Token const*, Type const*, Type_seq const&, Stmt const*);

  Stmt const* on_expression_stmt(Token const*, Expr const*);
  Stmt const* on_assignment_stmt(Token const*, Token const*, Expr const*, Expr const*);
  Stmt const* on_declaration_stmt(Decl const*);
};


void     init_grammar();
Stmt_seq parse_file(Token_stream&);


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


} // namespace beaker


#endif
