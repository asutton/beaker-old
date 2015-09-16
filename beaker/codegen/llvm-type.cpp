// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "llvm.hpp"

#include "beaker/type.hpp"


namespace beaker
{

// -------------------------------------------------------------------------- //
//                            Type translation
//
// Translate Beaker types to LLVM types.

void llvm_type(Printer&, Type const*);


void
llvm_type(Printer& p, Void_type const* t)
{
  print(p, "void");
}


// Because bool and int share the same value format,
// we represent them using the same type.
void
llvm_type(Printer& p, Boolean_type const* t)
{
  print(p, "i32");
}


// There is only one integer type.
void
llvm_type(Printer& p, Integer_type const* t)
{
  print(p, "i32");
}


// An LLVM function type has the spelling
//
//    (t1, t2, ..., tn) tr
//
// Where ti is the type of each parameter and tr is the
// return type of the function.
void
llvm_type(Printer& p, Function_type const* t)
{
  auto iter = t->parameter_types().begin();
  auto end = t->parameter_types().end();
  print(p, '(');
  while (iter != end) {
    llvm_type(p, *iter);
    if (std::next(iter) != end)
      print(p, ',');
    ++iter;
  }
  print(p, ')');
}


// Translate reference types to pointers.
void
llvm_type(Printer& p, Reference_type const* t)
{
  llvm_type(p, t->type());
  print(p, "*");
}


void
llvm_type(Printer& p, Type const* t)
{
  struct Fn
  {
    Fn(Printer& p)
      : p(p)
    { }

    void operator()(Void_type const* t) const { return llvm_type(p, t); }
    void operator()(Boolean_type const* t) const { return llvm_type(p, t); }
    void operator()(Integer_type const* t) const { return llvm_type(p, t); }
    void operator()(Function_type const* t) const { return llvm_type(p, t); }
    void operator()(Reference_type const* t) const { return llvm_type(p, t); }

    Printer& p;
  };

  return apply(t, Fn(p));
}


} // namespace beaker
