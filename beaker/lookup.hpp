// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_LOOKUP_HPP
#define BEAKER_LOOKUP_HPP

// The scope module provides a facility for managing the lexical
// scope of name bindings.re extensibility.

#include "beaker/prelude.hpp"


namespace beaker
{

struct Scope;


// The scope kind determines what kinds of "parts of a program"
// in which a name binding could be introduced. These are:
//
// - global -- anything not declared within a block or record
//   has global scope.
//
// - local -- any declarations within a block statement have
//   block scope. Parameters of a function definition are
//   declared in a block scope encloses its body.
//
// - function -- any declraations within function scope are
//   visible from their point of declaration to the end of
//   the function definition. Only parameters have function
//   scope.
enum Scope_kind
{
  global_scope,
  local_scope,
  function_scope,
};


// A (lexical) scope is a set of declartions that are visible
// within the same region of source, starting from their
// respective point of declarations.
struct Scope : std::vector<String const*>
{
  struct Binding;

  Scope(Scope_kind k);
  ~Scope();

  Scope_kind kind() const { return kind_; }
  
  Decl const* bind(String const*, Decl const*);
  Decl const* lookup(String const*) const;

  Scope_kind kind_;
};


// Used to declare the global scope.
struct Global_scope : Scope
{
  Global_scope()
    : Scope(global_scope)
  { }
};


// Used to declare a local scope.
struct Local_scope : Scope
{
  Local_scope()
    : Scope(local_scope)
  { }
};


// Used to declare a function scope.
struct Function_scope : Scope
{
  Function_scope()
    : Scope(function_scope)
  { }
};



// A scope entry stores information information associated
// with an identifier. This includes the bound declration,
// it's scope, and a reference to the previous bound entry 
// for the identifier.
struct Scope::Binding
{
  Decl const* decl;  // The bound declaration
  Scope*      scope; // The scope of the declaration
  Binding*    prev;  // A previous binding for the name
};


// ---------------------------------------------------------------------------//
//                            Scope management

Scope* current_scope();


// ---------------------------------------------------------------------------//
//                            Lexical bindings

bool declare(String const*, Decl const*);
bool declare(Decl const*);

Decl const* lookup(String const*);
Decl const* lookup(String const&);
Decl const* lookup(char const*);



} // namespace beaker


#endif
