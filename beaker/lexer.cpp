// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/lexer.hpp"

#include "lingo/lexing.hpp"
#include "lingo/symbol.hpp"

#include <cassert>
#include <cctype>
#include <iostream>
#include <string>


using namespace lingo;

namespace beaker
{

namespace
{

// Consume until the end of line.
void
comment(Lexer& lex, Character_stream& cs, Location loc)
{
  char const* init = cs.begin();
  while (next_element_is_not(cs, '\n'))
    cs.get();
  lex.on_comment(loc, init, cs.begin());
}


// Lexically analyze a single token.
Token
token(Lexer& lex, Character_stream& cs)
{
  while (!cs.eof()) {
    Location loc = cs.location();
    switch (cs.peek()) {
    case ' ':
    case '\t':
    case '\n':
      // Consume the WS and continue lexing.
      cs.get();
      break;

    case '{': return lex.on_lexeme(loc, &cs.get(), 1);
    case '}': return lex.on_lexeme(loc, &cs.get(), 1);
    case '(': return lex.on_lexeme(loc, &cs.get(), 1);
    case ')': return lex.on_lexeme(loc, &cs.get(), 1);
    case '[': return lex.on_lexeme(loc, &cs.get(), 1);
    case ']': return lex.on_lexeme(loc, &cs.get(), 1);
    case ';': return lex.on_lexeme(loc, &cs.get(), 1);
    case ':': return lex.on_lexeme(loc, &cs.get(), 1);
    case ',': return lex.on_lexeme(loc, &cs.get(), 1);
    
    case '<': 
      if (nth_element_is(cs, 1, '='))
        return lex.on_lexeme(loc, get_n(cs, 2), 2);
      else if (nth_element_is(cs, 1, '<'))
        return lex.on_lexeme(loc, get_n(cs, 2), 2);
      else
        return lex.on_lexeme(loc, &cs.get(), 1);

    case '>':
      if (nth_element_is(cs, 1, '='))
        return lex.on_lexeme(loc, get_n(cs, 2), 2);
      if (nth_element_is(cs, 1, '>'))
        return lex.on_lexeme(loc, get_n(cs, 2), 2);
      else
        return lex.on_lexeme(loc, &cs.get(), 1);
    
    case '=': 
      if (nth_element_is(cs, 1, '='))
        return lex.on_lexeme(loc, get_n(cs, 2), 2);
      else
        return lex.on_lexeme(loc, &cs.get(), 1);
    
    case '!': 
      if (nth_element_is(cs, 1, '='))
        return lex.on_lexeme(loc, get_n(cs, 2), 2);
      else
        return lex.on_lexeme(loc, &cs.get(), 1);

    case '&': 
      if (nth_element_is(cs, 1, '&'))
        return lex.on_lexeme(loc, get_n(cs, 2), 2);
      else
        return lex.on_lexeme(loc, &cs.get(), 1);
    
    case '|': 
      if (nth_element_is(cs, 1, '|'))
        return lex.on_lexeme(loc, get_n(cs, 2), 2);
      else
        return lex.on_lexeme(loc, &cs.get(), 1);
    
    case '^': return lex.on_lexeme(loc, &cs.get(), 1);
    case '~': return lex.on_lexeme(loc, &cs.get(), 1);

    case '+': return lex.on_lexeme(loc, &cs.get(), 1);
    case '*': return lex.on_lexeme(loc, &cs.get(), 1);
    
    case '-': 
      if (nth_element_is(cs, 1, '>'))
        return lex.on_lexeme(loc, get_n(cs, 2), 2);
      else
        return lex.on_lexeme(loc, &cs.get(), 1);
    
    case '/': 
      // For comments, consume the comment and loop.
      if (nth_element_is(cs, 1, '/')) {
        comment(lex, cs, loc);
        break;
      }
      return lex.on_lexeme(loc, &cs.get(), 1);
    
    case '%': return lex.on_lexeme(loc, &cs.get(), 1);
    
    default:
      if (is_identifier_start(cs.peek()))
        return lex_identifier(lex, cs, loc);
      
      if (is_decimal_digit(cs.peek()))
        return lex_decimal_integer(lex, cs, loc);
      
      error(loc, "unrecognized character '{}'", cs.peek());
      cs.get();
    }
  }

  return {};
}


} // namespace


Token
Lexer::on_lexeme(Location loc, char const* str, int n)
{
  return Token(loc, str, n);
}


Token
Lexer::on_integer(Location loc, char const* first, char const* last, int)
{
  // The integer base is currently ignored.
  return Token(loc, integer_tok, first, last);
}


Token
Lexer::on_identifier(Location loc, char const* first, char const* last)
{
  // If [first, last) is a keyword, the resulting token will
  // be that keyword and not the identifier.
  return Token(loc, identifier_tok, first, last);
}


// TODO: Cache comments for the purpose of doc association later.
void
Lexer::on_comment(Location, char const*, char const*)
{
}


// Lex all tokens in the character stream.
Token_list 
lex(Buffer& buf)
{
  Use_buffer cxt(buf);
  Character_stream cs(buf);
  Lexer lexer;
  Token_list toks;
  while (Token tok = token(lexer, cs))
    toks.push_back(tok);
  return toks;
}


} // namespace beaker
