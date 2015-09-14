// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "unit.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/decl.hpp"
#include "beaker/stmt.hpp"
#include "beaker/unit.hpp"


namespace beaker
{

namespace
{


// -------------------------------------------------------------------------- //
//                            Type translation
//
// Translate Beaker types to LLVM types.

String llvm_type(Type const*);


String
llvm_type(Void_type const* t)
{
  return "void";
}


// Because bool and int share the same value format,
// we represent them using the same type.
String
llvm_type(Boolean_type const* t)
{
  return "i32";
}


String
llvm_type(Integer_type const* t)
{
  return "i32";
}


// An LLVM function type has the spelling
//
//    (t1, t2, ..., tn) tr
//
// Where ti is the type of each parameter and tr is the
// return type of the function.
String
llvm_type(Function_type const* t)
{
  std::stringstream ss;
  auto iter = t->parameter_types().begin();
  auto end = t->parameter_types().end();
  while (iter != end) {
    ss << llvm_type(*iter);
    if (std::next(iter) != end)
      ss << ", ";
    ++iter;
  }
  ss << ' ' << llvm_type(t);
  return ss.str();
}


// Translate reference types to pointers.
String
llvm_type(Reference_type const* t)
{
  std::stringstream ss;
  ss << llvm_type(t->type()) << " *";
  return ss.str();
}


String
llvm_type(Type const* t)
{
  struct llvm_type_fn
  {
    String operator()(Void_type const* t) const { return llvm_type(t); }
    String operator()(Boolean_type const* t) const { return llvm_type(t); }
    String operator()(Integer_type const* t) const { return llvm_type(t); }
    String operator()(Function_type const* t) const { return llvm_type(t); }
    String operator()(Reference_type const* t) const { return llvm_type(t); }
  };
  return apply(t, llvm_type_fn());
}


// -------------------------------------------------------------------------- //
//                            Value translation
//
// Translate a constant value to an LLVM value.


// For now, values are just 32-bit integers. Write
// it as we see it.
String
llvm_value(Expr const* e)
{
  // FIXME: If the initializer is non-trivial, then we need to
  // emit it as an assignment within then unit constructor.
  //
  // TODO: We actually need to reduce this expression.
  //
  // TODO: With compound data types, we need to also translate
  // constant values to LLVM values.
  if (!is<Constant_expr>(e)) {
    error(e->location(), "non-constant global initialization not implemented");
    return "<error>";
  }

  std::stringstream ss;
  ss << cast<Constant_expr>(e)->value();
  return ss.str();
}


// -------------------------------------------------------------------------- //
//                            Top-level declarations
//
// Translate variable and function declarations to LLVM.


void
llvm_global(Printer& p, Variable_decl const* d)
{
  String type = llvm_type(d->type());
  String value = llvm_value(d->initializer());
  print(p, "@{} = global {} {}", d->name(), type, value);
  print_newline(p);
}


void
llvm_parm(Printer& p, Parameter_decl const* d)
{
  print(p, "{} %{}", llvm_type(d->type()), d->name());
}


void
llvm_parm_list(Printer& p, Function_decl const* d)
{
  print(p, '(');
  auto iter = d->parameters().begin();
  auto end = d->parameters().end();
  while (iter != end) {
    llvm_parm(p, cast<Parameter_decl>(*iter));
    if (std::next(iter) != end)
      print(p, ", ");
    ++iter;
  }
  print(p, ')');
}


// TODO: Implement this function.
void
llvm_function_def(Printer& p, Function_decl const* d)
{
  print(p, '{');
  if (is_void_type(d->return_type())) {
    print(p, "ret void");
  } else {
    print(p, "ret i32 0");
  }
  print(p, '}');
}


// Emit a function definition.
void
llvm_global(Printer& p, Function_decl const* d)
{
  String type = llvm_type(d->return_type());
  print(p, "define {} @{}", type, d->name(), type);
  llvm_parm_list(p, d);
  llvm_function_def(p, d);
  print_newline(p);
}


void
llvm_global(Printer& p, Decl const* d)
{
  struct llvm_toplevel_fn
  {
    llvm_toplevel_fn(Printer& p)
      : p(p)
    { }

    void operator()(Variable_decl const* d) const { llvm_global(p, d); }
    void operator()(Function_decl const* d) const { llvm_global(p, d); }
    
    // A parameter cannot be a top-level declaration.
    void operator()(Parameter_decl const* d) const { lingo_unreachable(); }

    Printer& p;
  };

  apply(d, llvm_toplevel_fn(p));
}


} // namespace

// -------------------------------------------------------------------------- //
//                                Translation units


void
to_llvm(std::ostream&, Unit const* u)
{
  Printer p(std::cout); // FIXME: Use an output file.
  for (Decl const* d : u->declarations()) {
    llvm_global(p, d);
  }
}



} // namespace beaker
