// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "llvm.hpp"

#include "beaker/stmt.hpp"


namespace beaker
{

// -------------------------------------------------------------------------- //
//                     Translation to LLVM statements

void llvm_expr(Printer&, Expr const*);
void llvm_stmt(Printer&, Stmt const*);


template<typename T>
void
llvm_stmt(Printer& p, T const* s)
{
  error(s->location(), "codegen for {} not implemneted", get_node_name(s));
}


// An empty statement results in no code.
inline void
llvm_stmt(Printer& p, Empty_stmt const*)
{
}


void
llvm_stmt(Printer& p, Exit_stmt const*)
{
  print(p, "ret void");
}


void
llvm_stmt(Printer& p, Return_stmt const* s)
{
  print(p, "ret i32 ");
  llvm_expr(p, s->result());
}


// Emit a new basic block.
void
llvm_stmt(Printer& p, Block_stmt const* s)
{
  // FIXME: Generate a unique integer id for each block within
  // the current function. Right now we're just using the pointer
  // value, which is unique but uninteresting.
  //
  // Print the label in the first column.
  print(p, '\n');
  print(p, "bb{}:", (std::uintptr_t)s);

  // Print each statement in turn.
  for (Stmt const* s1 : s->statements()) {
    print_newline(p);
    llvm_stmt(p, s1);
  }
}


void
llvm_stmt(Printer& p, Stmt const* s)
{
  struct Fn
  {
    Fn(Printer& p)
      : p(p)
    { }

    void operator()(Empty_stmt const* s) { llvm_stmt(p, s); }
    void operator()(Declaration_stmt const* s) { llvm_stmt(p, s); }
    void operator()(Expression_stmt const* s) { llvm_stmt(p, s); }
    void operator()(Assignment_stmt const* s) { llvm_stmt(p, s); }
    void operator()(If_then_stmt const* s) { llvm_stmt(p, s); }
    void operator()(If_else_stmt const* s) { llvm_stmt(p, s); }
    void operator()(While_stmt const* s) { llvm_stmt(p, s); }
    void operator()(Do_stmt const* s) { llvm_stmt(p, s); }
    void operator()(Exit_stmt const* s) { llvm_stmt(p, s); }
    void operator()(Return_stmt const* s) { llvm_stmt(p, s); }
    void operator()(Block_stmt const* s) { llvm_stmt(p, s); }

    Printer& p;
  };

  return apply(s, Fn(p));
}


} // namespace beaker
