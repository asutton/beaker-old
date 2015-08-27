// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_VARIABLE_HPP
#define BEAKER_VARIABLE_HPP

// This module provides facilities for the definition
// and checking of variables.

#include "beaker/prelude.hpp"

namespace beaker
{

bool check_initializer(Type const*, Expr const*);

} // namespace

#endif
