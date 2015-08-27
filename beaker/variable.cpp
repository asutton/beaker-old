// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/variable.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/same.hpp"


namespace beaker
{

// The type of the initializer (`e`) shall match the 
// declared type of the variable (`t`).
bool 
check_initializer(Type const* t, Expr const* e)
{
  return same(e->type(), t);
}


} // namespace
