// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_TYPE_HPP
#define BEAKER_TYPE_HPP

#include "beaker/prelude.hpp"

#include "lingo/node.hpp"


namespace beaker
{

struct Type_visitor;


// -------------------------------------------------------------------------- //
//                                Types

// The Type class represents the set of all types defined
// by and definable within the cmin language.
struct Type
{
  virtual ~Type() { }

  virtual void accept(Type_visitor&) const = 0;

  String node_name() const;
};


// The type visitor.
struct Type_visitor
{
  virtual void visit(Void_type const* t) { }
  virtual void visit(Boolean_type const* t) { }
  virtual void visit(Integer_type const* t) { }
  virtual void visit(Function_type const* t) { }
  virtual void visit(Reference_type const* t) { }
};


// The type of boolean values.
struct Void_type : Type
{
  void accept(Type_visitor& v) const { v.visit(this); }
};


// The type of boolean values.
struct Boolean_type : Type
{
  void accept(Type_visitor& v) const { v.visit(this); }
};


// The integer type represents the signed system integer 
// type. This is typically a 32-bit 2's complement
// integer.
//
// TODO: Allow the integer precision to be specified
// by configuration.
struct Integer_type : Type
{
  Integer_type()
    : prec_(32)
  { }

  void accept(Type_visitor& v) const { v.visit(this); }

  int precision() const { return prec_; }

  int prec_;
};


// A function type is that of a mapping of a sequence of
// input types to an output type.
struct Function_type : Type
{
  Function_type(Type_seq const& a, Type const* r)
    : first(a), second(r)
  { }

  void accept(Type_visitor& v) const { v.visit(this); }

  Type_seq const& parameter_types() const { return first; }
  Type const*     return_type() const     { return second; }

  Type_seq    first;
  Type const* second;
};


// A reference to an object of a given type.
struct Reference_type : Type
{
  Reference_type(Type const* t)
    : first(t)
  { }

  void accept(Type_visitor& v) const { v.visit(this); }

  Type const* type() const { return first; }

  Type const* first;
};


// -------------------------------------------------------------------------- //
//                               Queries


// Returns ture if `t` is the void type.
inline bool
is_void_type(Type const* t)
{
  return is<Void_type>(t);
}


// Returns ture if `t` is the boolean type.
inline bool
is_boolean_type(Type const* t)
{
  return is<Boolean_type>(t);
}


// Returns true if `t` is the integer type.
inline bool 
is_integer_type(Type const* t)
{
  return is<Integer_type>(t);
}


// Returns true if `t` is a reference type.
inline bool
is_reference_type(Type const* t)
{
  return is<Reference_type>(t);
}


// Returns true if `t` is the type of an object. The
// object types are `bool` and `int`.
inline bool
is_object_type(Type const* t)
{
  return is_boolean_type(t) || is_integer_type(t);
}


// -------------------------------------------------------------------------- //
//                               Type accessors

Void_type const*      get_void_type();
Boolean_type const*   get_bool_type();
Integer_type const*   get_int_type();
Function_type const*  get_function_type(Type_seq const&, Type const*);
Function_type const*  get_function_type(Decl_seq const&, Type const*);
Reference_type const* get_reference_type(Type const*);


// -------------------------------------------------------------------------- //
//                            Generic visitor


// A parameterized visitor that dispatches to a function 
// object. F is the type of the function and T is its
// return type.
//
// This class is never used directly. It is used only in
// the apply function below.
template<typename F, typename T>
struct Generic_type_visitor : Type_visitor, Generic_visitor<F, T>
{
  Generic_type_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Void_type const* t) { this->invoke(t); }
  void visit(Boolean_type const* t) { this->invoke(t); }
  void visit(Integer_type const* t) { this->invoke(t); }
  void visit(Function_type const* t) { this->invoke(t); }
  void visit(Reference_type const* t) { this->invoke(t); }
};


// Apply the function f to the type t.
// The return type is that of the function object F.
template<typename F, typename T = typename std::result_of<F(Void_type*)>::type>
inline T
apply(Type const* t, F fn)
{
  Generic_type_visitor<F, T> v(fn);
  return accept(t, v);
}


} // namespace beaker


#endif

