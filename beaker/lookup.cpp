// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/lookup.hpp"
#include "beaker/decl.hpp"

#include "lingo/symbol.hpp"

#include <forward_list>
#include <unordered_map>
#include <iostream>


namespace beaker
{

namespace
{

// -------------------------------------------------------------------------- //
//                          Lexical environment

// The (name) environment provides a global mapping of 
// names to declarations.
struct Environment : std::unordered_map<String const*, Scope::Binding*>
{
  Decl const* push(String const*, Scope*, Decl const*);
  void        pop(String const*);

  Scope::Binding* binding(String const*) const;
};


// Push a new name binding into the context. Because we don't
// support overloading, it must be the case that insertion
// always succeeds.
Decl const*
Environment::push(String const* n, Scope* s, Decl const* d)
{
  auto ins = insert({n, nullptr});

  // Get an alias to the binding. Note that we update
  // this below.
  Scope::Binding*& b = ins.first->second;

  // Ensure that we aren't creating multiple declarations
  // in the same scope.
  lingo_assert(!ins.second ? b->scope != s : true);

  // Chain the new binding to the previous and update.
  b = new Scope::Binding{d, s, b};
  
  return d;
}


// Pop the innermost binding for this name. If this we pop
// all bindings, then remove the entry altogether. Note
// that `n` shall have a binding.
void
Environment::pop(String const* n)
{
  // Guarantee that we have a binding for `n`.
  auto iter = find(n);
  lingo_assert(iter != end());
  
  // If we've found the last binding erase the entry so
  // we don't have "partially" bound identifiers.
  Scope::Binding* e = iter->second;
  if (e->prev)
    iter->second = e->prev;
  else
    erase(iter);
  
  delete e;
}


// Returns the binding for the name `n`. If `n` has no current
// binding, returns nullptr.
Scope::Binding*
Environment::binding(String const* n) const
{
  auto iter = find(n);
  if (iter != end())
    return iter->second;
  else
    return nullptr;
}

// The global naming environment.
Environment env_;


// -------------------------------------------------------------------------- //
//                              Scope stack


// The scope stack is a stack of scopes.
struct Stack : std::forward_list<Scope*>
{
  Scope*       top()       { return front(); }
  Scope const* top() const { return front(); }
};


// The global scope stack.
Stack       stack_;


} // namespace


// When constructing a scope, place it on the scope stack.
Scope::Scope(Scope_kind k)
  : kind_(k)
{
  stack_.push_front(this);
}


// Pop all scope bindings and leave the scope whenever
// a scope is destroyed.
Scope::~Scope()
{
  for (String const* s : *this)
    env_.pop(s);
  stack_.pop_front();
}


// Push a new, innermost binding for the given name.
//
// Note that `n` shall not have been previously declared
// in this scope. This should be enforced by the `declare()`
// function.
Decl const*
Scope::bind(String const* n, Decl const* d)
{
  push_back(n);
  return env_.push(n, this, d);
}


// Returns the innermost declaration of `s` or nullptr 
// if no such declaration exists.
Decl const*
Scope::lookup(String const* s) const
{
  if (Scope::Binding* b = env_.binding(s))
    return b->decl;
  else
    return nullptr;
}



// -------------------------------------------------------------------------- //
//                           Scope management

// Returns the current stack.
Scope*
current_scope()
{
  return stack_.top();
}


// -------------------------------------------------------------------------- //
//                             Declarations

namespace
{

// Returns true if `n` does not hide an existing declaration
// in the same scope. Otherwise, emits a diagnostic and returns
// false.
bool
check_redeclaration(String const* n, Decl const* decl)
{
  Scope* s = current_scope();
  if (Scope::Binding* b = env_.binding(n))
    if (b->scope == s) {
      Decl const* prev = b->decl;
      error(decl->location(), "'{}' is already declared", n);

      // This isn't meaningful if there's no source location.
      // TODO: If the previous declaration is in a different
      // source file, we would need to re-establish the input
      // context in order to get the right location.
      if (prev->location())
        note(prev->location(), "previous declaration here", prev);
      
      return false;
    }
  return true;
}


} // namespace


// Declares `s` to be the declaration `d`. This associates
// the type and meaning (value or function) of `d` with `s` 
// in the current scope. Returns `true` if the declaration
// is valid.
//
// Note that `s` must be a string in the symbol table.
bool
declare(String const* s, Decl const* d)
{
  if (check_redeclaration(s, d)) {
    current_scope()->bind(s, d);
    return true;
  }
  return false;
}


// Save the declaration `d` in the current scope. This
// is equivalent to `declare(d->name(), d)`.
bool 
declare(Decl const* d)
{
  return declare(d->name(), d);
}


// -------------------------------------------------------------------------- //
//                             Name lookup

// Returns the declaration bound `name` or nullptr if
// no such declaration exists.
Decl const*
lookup(String const* name)
{
  return current_scope()->lookup(name);
}


// Returns the declaration bound to the given name, or nullptr
// if no such declaration exists.
Decl const*
lookup(char const* name)
{
  return lookup(get_string(name));
}


} // namespace beaker
