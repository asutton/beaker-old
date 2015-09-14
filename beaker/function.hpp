// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_FUNCTION_HPP
#define BEAKER_FUNCTION_HPP

// This module provides facilities for the definition
// and checking of functions.

#include "beaker/prelude.hpp"

namespace beaker
{

bool check_definition(Type const*, Stmt const*);
bool check_definition(Function_decl const*, Stmt const*);

bool check_arguments(Function_type const*, Expr_seq const&);

} // namespace

#endif
