// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "llvm.hpp"

#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/decl.hpp"
#include "beaker/stmt.hpp"
#include "beaker/unit.hpp"


namespace beaker
{

// FIXME: Make these take printers just like everything else?
void llvm_type(Printer&, Type const*);
void llvm_stmt(Printer&, Stmt const*);

namespace
{

// -------------------------------------------------------------------------- //
//                            Top-level declarations
//
// Translate variable and function declarations to LLVM.


// Emit the global initializer.
//
// FIXME: Non-trivial initializers need special handling. Each
// initializer becomes a special function with internal linkage
// that assigns a value to the global variable.
void
llvm_global_init(Printer& p, Expr const* e)
{
  // Just emit 0 and continue processing.
  if (!is<Constant_expr>(e)) {
    error(e->location(), "non-constant global initialization not implemented");
    print(p, 0);
    return;
  }

  // Print the constant value.
  //
  // TODO: With compound data types, we need to also translate
  // constant values to LLVM values.
  Constant_expr const* c = cast<Constant_expr>(e);
  print(p, c->value());
}


void
llvm_global(Printer& p, Variable_decl const* d)
{
  print(p, "@{} = global ");
  llvm_type(p, d->type());
  llvm_global_init(p, d->initializer());
  print_newline(p);
}


void
llvm_parm(Printer& p, Parameter_decl const* d)
{
  llvm_type(p, d->type());
  print_space(p);
  print(p, d->name());
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
  indent(p);
  llvm_stmt(p, d->body());
  undent(p);
  print_newline(p);
  print(p, '}');
}


// Emit a function definition.
void
llvm_global(Printer& p, Function_decl const* d)
{
  print(p, "define ");
  llvm_type(p, d->return_type());
  print(p, "@{}", d->name());
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
