// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/parse.hpp"


namespace beaker
{


Type const* parse_type(Parser&, Token_stream&);


// Parse a type.
//
//    type ::= 'void' | 'bool' | 'int'
Type const*
parse_type(Parser& p, Token_stream& ts)
{
  switch (next_token_kind(ts)) {
    case void_kw:
      return p.on_void_type(get_token(ts));
    case bool_kw:
      return p.on_bool_type(get_token(ts));
    case int_kw:
      return p.on_int_type(get_token(ts));
    default:
      break;
  }
  
  error(ts.location(), "invalid type '{}'", ts.peek());
  return make_error_node<Type>();
}


} // namepace beaker
