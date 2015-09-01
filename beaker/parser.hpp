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
};


void     init_grammar();
Stmt_seq parse_file(Token_stream&);



} // namespace beaker


#endif
