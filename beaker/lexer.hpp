// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_LEXER_HPP
#define BEAKER_LEXER_HPP

// The lexer module implements the lexical analyzer for
// the programming language.

#include "beaker/prelude.hpp"
#include "beaker/token.hpp"

#include "lingo/character.hpp"


namespace beaker
{

// The Lexer class defines the actions taken whenever
// a sequence of characters as a particular kind of token.
struct Lexer
{
  using argument_type = char;
  using result_type = Token;

  // Semantic actions.
  Token on_lexeme(Location, char const*, int);
  Token on_identifier(Location, char const*, char const*);
  Token on_integer(Location, char const*, char const*, int);

  void on_comment(Location, char const*, char const*);
};


Token_list lex(Buffer&);


} // namespace beaker


#endif
