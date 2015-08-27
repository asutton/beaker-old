// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_FUNCTION_HPP
#define BEAKER_FUNCTION_HPP

// This module provides facilities for the definition
// and checking of functions.

#include "beaker/prelude.hpp"

namespace beaker
{

bool check_return_type(Type const* t, Stmt const* s);

} // namespace

#endif
