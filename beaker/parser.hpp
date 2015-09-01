// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef STEVE_PARSER_HPP
#define STEVE_PARSER_HPP

// The parser module defines the syntactic analysis of
// the Steve grammar.

#include "steve/prelude.hpp"
#include "steve/token.hpp"
#include "steve/type.hpp"

#include "lingo/parsing.hpp"

namespace steve
{

// A sequence of function arguments.
using Arg_seq = Sequence_term<Expr>;


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
  Expr const* on_call_expr(Token const*, Expr const*, Expr_seq const*);
};


void     init_grammar();
Stmt_seq parse_file(Token_stream&);



} // namespace steve


#endif
