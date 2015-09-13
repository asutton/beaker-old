// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef STEVE_STMT_HPP
#define STEVE_STMT_HPP

// The statement module defines the notions of statements
// in the language.

#include "beaker/prelude.hpp"

#include "lingo/node.hpp"


namespace beaker
{

struct Stmt_visitor;


// The base class of all statements in the language. A statement 
// contains a declaration, an expression, or it represents a
// form of flow control.
struct Stmt
{
  virtual ~Stmt() { }

  String node_name() const;

  // Accept a statement visitor.
  virtual void accept(Stmt_visitor&) const = 0;
  
  // Returns the source location where the statement begins.
  virtual Location location() const = 0;
};


struct Stmt_visitor
{
  virtual void visit(Empty_stmt const*) { }
  virtual void visit(Declaration_stmt const*) { }
  virtual void visit(Expression_stmt const*) { }
  virtual void visit(Assignment_stmt const*) { }
  virtual void visit(If_then_stmt const*) { }
  virtual void visit(If_else_stmt const*) { }
  virtual void visit(While_stmt const*) { }
  virtual void visit(Do_stmt const*) { }
  virtual void visit(Return_stmt const*) { }
  virtual void visit(Block_stmt const*) { }
};


// The empty statement invokes no commands.
struct Empty_stmt : Stmt
{
  Empty_stmt(Location loc)
    : loc_(loc)
  { }

  void accept(Stmt_visitor& v) const { v.visit(this); }
  
  Location location() const { return loc_; }

  Location loc_; // Location of ';'
};


// A declaration statement contains a declaration.
struct Declaration_stmt : Stmt
{
  Declaration_stmt(Decl const* d)
    : first(d)
  { }

  void accept(Stmt_visitor& v) const { v.visit(this); }

  Location location() const;

  Decl const* decl() const { return first; }

  Decl const* first;
};


// An expression statement executes its expression
// and discards the result. The runtime environment
// may save the result in a log file (or terminal).
struct Expression_stmt : Stmt
{
  Expression_stmt(Location l, Expr const* e)
    : loc_(l), first(e)
  { }

  void accept(Stmt_visitor& v) const { v.visit(this); }

  Location location() const;
  Location semicolon_location() const;

  Expr const* expr() const { return first; }

  Location    loc_;   // The ';' location
  Expr const* first;
};


/* Represents an assignment statement. */
struct Assignment_stmt : Stmt
{
  Assignment_stmt(Location loc, Expr const* l, Expr const* r)
    : loc_(loc), first(l), second(r)
  { }
  
  void accept(Stmt_visitor& v) const { v.visit(this); }

  Location location() const;
  Location assign_location() const { return loc_; }

  Expr const* lhs() const { return first; }
  Expr const* rhs() const { return second; }

  Location    loc_;   // The '=' token.
  Expr const* first;
  Expr const* second;
};


struct If_then_stmt : Stmt
{
  If_then_stmt(Location loc, Expr const* c, Stmt const* b)
    : loc_(loc), first(c), second(b)
  { }

  void accept(Stmt_visitor& v) const { v.visit(this); }

  Location    location() const  { return loc_; }
  Expr const* condition() const { return first; }
  Stmt const* branch() const    { return second; }

  Location    loc_;   // The location of 'if'.
  Expr const* first;  // The branch condition
  Stmt const* second; // The true branch
};


struct If_else_stmt : Stmt
{
  If_else_stmt(Location l1, Location l2, Expr const* c, Stmt const* t, Stmt const* f)
    : if_(l1), else_(l2), first(c), second(t), third(f)
  { }

  void accept(Stmt_visitor& v) const { v.visit(this); }

  Location    location() const      { return if_location(); }
  Location    if_location() const   { return if_; }
  Location    else_location() const { return else_; }
  
  Expr const* condition() const     { return first; }
  Stmt const* true_branch() const   { return second; }
  Stmt const* false_branch() const  { return third; }

  Location    if_;    // Location of 'if'
  Location    else_;  // Location of 'else'
  Expr const* first;  // The condition
  Stmt const* second; // The true branch
  Stmt const* third;  // The false branch
};


// Represents a while-statement.
struct While_stmt : Stmt
{
  While_stmt(Location loc, Expr const* c, Stmt const* b)
    : loc_(loc), first(c), second(b)
  { }

  void accept(Stmt_visitor& v) const { v.visit(this); }

  Location    location() const  { return loc_; }
  Expr const* condition() const { return first; }
  Stmt const* body() const      { return second; }

  Location    loc_;   // The location of 'while'.
  Expr const* first;  // The loop condition
  Stmt const* second; // The loop body
};


// Represents a do-while-statement.
struct Do_stmt : Stmt
{
  Do_stmt(Location l1, Location l2, Expr const* c, Stmt const* b)
    : do_(l1), while_(l2), first(c), second(b)
  { }

  void accept(Stmt_visitor& v) const { v.visit(this); }

  Location    location() const       { return do_location(); }
  Location    do_location() const    { return do_; }
  Location    while_location() const { return while_; }
  
  Expr const* condition() const      { return first; }
  Stmt const* body() const           { return second; }

  Location    do_;    // Location of 'do' and 'while'.
  Location    while_; // Location of 'while'
  Expr const* first;  // The condition
  Stmt const* second; // The loop body
};


// Represents a return-statement.
struct Return_stmt : Stmt
{
  Return_stmt(Location l1, Location l2, Expr const* e)
    : ret_(l1), semi_(l2), first(e)
  { }

  void accept(Stmt_visitor& v) const { v.visit(this); }

  Location location() const           { return return_location(); }
  Location return_location() const    { return ret_; }
  Location semicolon_location() const { return semi_; }
  
  Expr const* result() const { return first; }

  Location    ret_;  // Location of 'return'
  Location    semi_; // Location of ';'
  Expr const* first;
};


// A block statement is a sequence of statements. 
struct Block_stmt : Stmt
{
  // TODO: Support move semantics for the statement sequence.
  Block_stmt(Location l1, Location l2, Stmt_seq const& s)
    : open_(l1), close_(l2), first(s)
  { }

  void accept(Stmt_visitor& v) const { v.visit(this); }

  Location location() const       { return open_location(); }
  Location open_location() const  { return open_; }
  Location close_location() const { return close_; }

  Stmt_seq const& statements() const { return first; }

  Location open_;   // Location of '{'
  Location close_;  // Location of '}'
  Stmt_seq first;   // Sequence of statements.
};


// -------------------------------------------------------------------------- //
//                            Statement builders

Empty_stmt*       make_empty_stmt(Location);
Declaration_stmt* make_declaration_stmt(Decl const*);
Expression_stmt*  make_expression_stmt(Location, Expr const*);
Assignment_stmt*  make_assign_stmt(Location, Expr const*, Expr const*);
If_then_stmt*     make_if_then_stmt(Location, Expr const*, Stmt const*);
If_else_stmt*     make_if_else_stmt(Location, Location, Expr const*, Stmt const*, Stmt const*);
While_stmt*       make_while_stmt(Location, Expr const*, Stmt const*);
Do_stmt*          make_do_stmt(Location, Location, Expr const*, Stmt const*);
Return_stmt*      make_return_stmt(Location, Location, Expr const*);
Block_stmt*       make_block_stmt(Location, Location, Stmt_seq const&);


inline Empty_stmt*
make_empty_stmt()
{
  return make_empty_stmt({});
}


inline Expression_stmt*
make_expression_stmt(Expr const* e)
{
  return make_expression_stmt({}, e);
}


inline Return_stmt*
make_return_stmt(Expr const* e)
{
  return make_return_stmt({}, {}, e);
}

inline Block_stmt*
make_block_stmt(Stmt_seq const& seq)
{
  return make_block_stmt({}, {}, seq);
}


// -------------------------------------------------------------------------- //
//                            Generic visitor

// A parameterized visitor that dispatches to a function object.
template<typename F, typename T>
struct Generic_stmt_visitor : Stmt_visitor, Generic_visitor<F, T>
{
  Generic_stmt_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Empty_stmt const* s) { return this->invoke(s); }
  void visit(Declaration_stmt const* s) { return this->invoke(s); }
  void visit(Expression_stmt const* s) { return this->invoke(s); }
  void visit(Assignment_stmt const* s) { return this->invoke(s); }
  void visit(If_then_stmt const* s) { return this->invoke(s); }
  void visit(If_else_stmt const* s) { return this->invoke(s); }
  void visit(While_stmt const* s) { return this->invoke(s); }
  void visit(Do_stmt const* s) { return this->invoke(s); }
  void visit(Return_stmt const* s) { return this->invoke(s); }
  void visit(Block_stmt const* s) { return this->invoke(s); }
};


// Apply the function f to the statement s.
template<typename F, typename T = typename std::result_of<F(Empty_stmt*)>::type>
inline T
apply(Stmt const* s, F fn)
{
  Generic_stmt_visitor<F, T> v(fn);
  return accept(s, v);
}


} // namespace beaker

#endif
