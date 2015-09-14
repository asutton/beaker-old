// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/decl.hpp"
#include "beaker/less.hpp"

#include "lingo/memory.hpp"


namespace beaker
{

// -------------------------------------------------------------------------- //
//                             Type accessors

// Global types.
namespace
{

// Compares type values.
struct Type_less
{
  bool operator()(Type const& a, Type const& b) const
  {
    return less(&a, &b);
  }
};


// Canonicalizing factories.
using Function_types = Unique_factory<Function_type, Type_less> ;
using Reference_types = Unique_factory<Reference_type, Type_less> ;


Void_type void_;
Boolean_type bool_;
Integer_type int_;
Function_types fn_;
Reference_types ref_;

} // namespace


Void_type const*
get_void_type()
{
  return &void_;
}


Boolean_type const*
get_bool_type()
{
  return &bool_;
}


Integer_type const*
get_int_type()
{
  return &int_;
}


Function_type const*
get_function_type(Type_seq const& t, Type const* r)
{
  return fn_.make(t, r);
}


// Returns the parameter type for a sequence of paramteers.
static Type_seq
get_parameter_types(Decl_seq const& p)
{
  Type_seq t;
  t.reserve(p.size());
  for (Decl const* d : p) {
    t.push_back(d->type());
  }
  return t;
}


Function_type const*
get_function_type(Decl_seq const& d, Type const* r)
{
  return fn_.make(get_parameter_types(d), r);
}


// Return a reference to the type t. 
Reference_type const*
get_reference_type(Type const* t)
{
  // If T is non-void, T& is a reference type.
  if (is_void_type(t)) {
    error("forming a reference to 'void'");
    return make_error_node<Reference_type>();
  }

  // If T is a reference type then T& is equivalent to T.
  if (Reference_type const* t1 = as<Reference_type>(t))
    return t1;

  return ref_.make(t);
}


// Returns the expression type. If the expression's type is
// T&, then the result is adjusted to T.
Type const*
get_expr_type(Expr const* e)
{
  Type const* t = e->type();
  if (Reference_type const* r = as<Reference_type>(t))
    return r->type();
  return t;
}


} // namespace beaker
