// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/parse.hpp"


namespace beaker
{


Decl const* parse_expr(Parser&, Token_stream&);
Decl const* parse_type(Parser&, Token_stream&);
Decl const* parse_decl(Parser&, Token_stream&);
Decl const* parse_stmt(Parser&, Token_stream&);


namespace
{


// Used to propagate diagnosed errors.
Decl const* error_decl = make_error_node<Decl>();


} // namespace


// Parse a declaration.
//
//    decl ::= variable-decl
//           | function-decl
Decl const*
parse_decl(Parser& p, Token_stream& ts)
{
  error(ts.location(), "not implemented");
  return make_error_node<Decl>();
}


} // namepace beaker
