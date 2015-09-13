// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef STEVE_LLVM_HPP
#define STEVE_LLVM_HPP

// This module is responsible for the translation of
// programs to the LLVM IR. Note that this generates
// Raw (textual) assembly rather than actual LLVM
// byte code.

#include "beaker/prelude.hpp"


namespace beaker
{

void to_llvm(std::ostream&, Unit const*);

} // namespace beaker


#endif
