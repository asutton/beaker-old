// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_TOKEN_HPP
#define BEAKER_TOKEN_HPP

// This module defines the set of tokens used by the language.

#include "beaker/prelude.hpp"

#include "lingo/token.hpp"

namespace beaker
{

using namespace lingo;


// -------------------------------------------------------------------------- //
//                              Token kinds

// The kinds of tokens used in beaker.
enum Token_kind
{
  // Punctuators
  lbrace_tok,     // {
  rbrace_tok,     // }
  lparen_tok,     // )
  rparen_tok,     // (
  colon_tok,      // :
  semicolon_tok,  // ;
  eq_tok,         // =
  arrow_tok,      // ->
  // Opeators
  plus_tok,       // +
  minus_tok,      // -
  star_tok,       // *
  slash_tok,      // /
  percent_tok,    // %
  amp_tok,        // &
  bar_tok,        // |
  caret_tok,      // ^
  tilde_tok,      // ~
  lt_lt_tok,      // <<
  gt_gt_tok,      // >>
  eq_eq_tok,      // ==
  bang_eq_tok,    // !=
  lt_tok,         // <
  gt_tok,         // >
  lt_eq_tok,      // <=
  gt_eq_tok,      // >=
  amp_amp_tok,    // &&
  bar_bar_tok,    // ||
  bang_tok,       // !
  // Keywords
  bool_kw,        // bool
  def_kw,         // def
  do_kw,          // do
  else_kw,        // else
  false_kw,       // false
  if_kw,          // if
  int_kw,         // int
  return_kw,      // return
  true_kw,        // true
  var_kw,         // var
  while_kw,       // while
  // Character classes
  identifier_tok, // identifiers
  integer_tok     // integers
};


String const* get_identifier(char const*);
String const* get_identifier(String const&);


void init_tokens();


} // namespace beaker


#endif
