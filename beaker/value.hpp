// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_VALUE_HPP
#define BEAKER_VALUE_HPP

// The value module defines the representation of values
// formed during the evaluation of expressions. 
//
// Because this version of the language deals only with
// integral values (boolean values can be represented as
// 1 or 0), we do not need a complex value representation.

#include "beaker/prelude.hpp"


namespace beaker
{

using Value = std::intmax_t;

} // namespace beaker


#endif
