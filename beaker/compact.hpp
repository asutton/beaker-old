// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_COMPACT_HPP
#define BEAKER_COMPACT_HPP

// This module defines the compaction of expressions. It takes
// an abstract syntax tree and produces a directed acyclic graph
// of that tree such that common terminals are represented by
// the same expression.


#include "beaker/prelude.hpp"


namespace beaker
{

Expr const* compact(Expr const*);


} // namespace beaker


#endif

