// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef STEVE_UNIT_HPP
#define STEVE_UNIT_HPP

// The unit module defines the top-level structure of modules
// or programs.

#include "beaker/prelude.hpp"

#include "lingo/node.hpp"


namespace beaker
{

struct Stmt_visitor;


// A translation unit is a sequence of top-level declarations
// comprising a module or program.
//
// There are (currently) no derived translation units.
struct Unit
{
  Unit(Decl_seq const& d)
    : first(d)
  { }

  Decl_seq const& declarations() const { return first; }

  Decl_seq first;
};


// Create a new translation unit from the sequence of declarations.
inline Unit const*
make_unit(Decl_seq const& d)
{
  return new Unit(d);
}


} // namespace beaker


#endif
