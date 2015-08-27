// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_DECL_HPP
#define BEAKER_DECL_HPP

// The declaration module defines the declarations
// in the language and tools for working with them.

#include "beaker/prelude.hpp"

#include "lingo/node.hpp"


namespace beaker
{

struct Decl_visitor;


// The Decl class is the base class of all declarations 
// in the language. A declaration binds a symbol (identifier)
// to its type, value, definition, etc.
//
// Every declaration has a name and type.
//
// Each declaration explicitly contains a source location,
// which represennts the lexical point at which the declaration 
// begins. Note that most declaratins are specifically 
// introduced by a keyword (e.g., `var`, or `def`).
struct Decl
{
  Decl(Location l, String const* n, Type const* t)
    : loc_(l), name_(n), type_(t)
  { }

  virtual ~Decl() { }

  // Accept a declaration visitor.
  virtual void accept(Decl_visitor&) const = 0;
  
  String        node_name() const;
  Location      location() const  { return loc_; }
  String const* name() const      { return name_; }
  Type const*   type() const      { return type_; }

  Location      loc_;   // The token that begins the declaration
  String const* name_;  // The bound identifier
  Type const*   type_;  // The bound type
};


// The declaration visitor.
struct Decl_visitor
{
  virtual void visit(Variable_decl const*) { }
  virtual void visit(Function_decl const*) { }
  virtual void visit(Parameter_decl const*) { }
};


// A variable declration introduces a named binding for a
// value. Once bound, the name cannot be rebound.
struct Variable_decl : Decl
{
  Variable_decl(Location loc, String const* n, Type const* t, Expr const* e)
    : Decl(loc, n, t), first(e)
  { }

  void accept(Decl_visitor& v) const { return v.visit(this); }

  Expr const* initializer() const { return first; }

  Expr const* first;
};


// A function declaration defines a mapping from a sequence
// of inputs to an output. The parameters of a function
// determine the types of inputs. The body of a function is
// a statement that computes the result.
struct Function_decl : Decl
{
  Function_decl(Location, String const*, Type const*, Decl_seq const&, Stmt const*);

  void accept(Decl_visitor& v) const { return v.visit(this); }

  Decl_seq const&      parameters() const { return first; }
  Stmt const*          body() const       { return second; }
  Function_type const* type() const;
  Type const*          return_type() const;

  Decl_seq    first;
  Stmt const* second;
};


// A parameter declaration.
struct Parameter_decl : Decl
{
  Parameter_decl(Location loc, String const* n, Type const* t)
    : Decl(loc, n, t)
  { }

  void accept(Decl_visitor& v) const { return v.visit(this); }
};


// -------------------------------------------------------------------------- //
//                            Declaration builders

Variable_decl*  make_variable_decl(Location, String const*, Type const*, Expr const*);
Function_decl*  make_function_decl(Location, String const*, Decl_seq const&, Type const*, Stmt const*);
Parameter_decl* make_parameter_decl(Location, String const*, Type const*);


// Make a new variable declaration. 
inline Variable_decl*
make_variable_decl(String const* n, Type const* t, Expr const* e)
{
  return make_variable_decl(Location::none, n, t, e);
}


// Make a function declaration.
inline Function_decl* 
make_function_decl(String const* n, Decl_seq const& p, Type const* r, Stmt const* b)
{
  return make_function_decl(Location::none, n, p, r, b);
}


// Make a parameter declaration.
inline Parameter_decl*
make_parameter_decl(String const* n, Type const* t)
{
  return make_parameter_decl(Location::none, n, t);
}


// -------------------------------------------------------------------------- //
//                            Generic visitor

// A parameterized visitor that dispatches to a function object.
template<typename F, typename T>
struct Generic_decl_visitor : Decl_visitor, Generic_visitor<F, T>
{
  Generic_decl_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Variable_decl const* d) { return this->invoke(d); }
  void visit(Function_decl const* d) { return this->invoke(d); }
  void visit(Parameter_decl const* d) { return this->invoke(d); }
};


// Apply the function f to the statement s.
template<typename F, typename T = typename std::result_of<F(Variable_decl*)>::type>
inline T
apply(Decl const* s, F fn)
{
  Generic_decl_visitor<F, T> v(fn);
  return accept(s, v);
}


} // namespace beaker

#endif
