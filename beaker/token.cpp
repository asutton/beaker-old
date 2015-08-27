// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/token.hpp"


namespace beaker
{

// Returns an identifer.
String const*
get_identifier(char const* str)
{
  return &lingo::get_symbol(str, identifier_tok).str;
}


// Returns an identifier.
String const*
get_identifier(String const& str)
{
  return &lingo::get_symbol(str, identifier_tok).str;
}


// Initialize language tokens.
void
init_tokens()
{
#define install(tok, txt) install_token(tok, #tok, txt)
  // Punctuators
  install(lbrace_tok,     "{");
  install(rbrace_tok,     "}");
  install(lparen_tok,     ")");
  install(rparen_tok,     "(");
  install(colon_tok,      ":");
  install(semicolon_tok,  ";");
  install(eq_tok,         "=");
  install(arrow_tok,      "->");
  // Opeators
  install(plus_tok,       "+");
  install(minus_tok,      "-");
  install(star_tok,       "*");
  install(slash_tok,      "/");
  install(percent_tok,    "%");
  install(amp_tok,        "&");
  install(bar_tok,        "|");
  install(caret_tok,      "^");
  install(tilde_tok,      "~");
  install(lt_lt_tok,      "<<");
  install(gt_gt_tok,      ">>");
  install(eq_eq_tok,      "==");
  install(bang_eq_tok,    "!=");
  install(lt_tok,         "<");
  install(gt_tok,         ">");
  install(lt_eq_tok,      "<=");
  install(gt_eq_tok,      ">=");
  install(amp_amp_tok,    "&&");
  install(bar_bar_tok,    "||");
  install(bang_tok,       "!");
  // Keywords
  install(bool_kw,        "bool");
  install(def_kw,         "def");
  install(do_kw,          "do");
  install(else_kw,        "else");
  install(if_kw,          "if");
  install(int_kw,         "int");
  install(return_kw,      "return");
  install(var_kw,         "var");
  install(while_kw,       "while");
#undef install
}


} // namespace beaker
