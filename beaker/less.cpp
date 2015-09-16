// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/less.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"

#include <typeindex>


namespace beaker
{

// Dispatch to an appropriate function.
struct Type_less_fn
{
  Type_less_fn(Type const* t)
    : t2(t)
  { }

  bool operator()(Void_type const* t1) const
  { 
    return less(t1, cast<Void_type>(t2)); 
  }
  
  bool operator()(Boolean_type const* t1) const
  { 
    return less(t1, cast<Boolean_type>(t2)); 
  }
  
  bool operator()(Integer_type const* t1) const
  { 
    return less(t1, cast<Integer_type>(t2)); 
  }
  
  bool operator()(Function_type const* t1) const
  { 
    return less(t1, cast<Function_type>(t2)); 
  }

  bool operator()(Reference_type const* t1) const
  { 
    return less(t1, cast<Reference_type>(t2)); 
  }

  Type const* t2;
};


// Returns true if one type is less than another.
bool 
less(Type const* a, Type const* b)
{
  // Abritrarily order types by their type information.
  std::type_index t1 = typeid(*a);
  std::type_index t2 = typeid(*b);
  if (t1 < t2)
    return true;
  if (t2 < t1)
    return false;

  // If two types are equivalent, then dispatch to
  // an appropriate function.
  return apply(a, Type_less_fn(b));
}


// Returns true if one type is less than another.
bool 
less(Expr const* a, Expr const* b)
{
  // Abritrarily order types by their type information.
  std::type_index t1 = typeid(*a);
  std::type_index t2 = typeid(*b);
  if (t1 < t2)
    return true;
  if (t2 < t1)
    return false;

  struct Fn
  {
    Fn(Expr const* b)
      : b(b)
    { }

    bool operator()(Constant_expr const* a) const 
    { 
      return less(a, cast<Constant_expr>(b)); 
    }

    bool operator()(Identifier_expr const* a) const 
    { 
      return less(a, cast<Identifier_expr>(b)); 
    }

    bool operator()(Unary_expr const* a) const 
    { 
      return less(a, cast<Unary_expr>(b)); 
    }

    bool operator()(Binary_expr const* a) const 
    { 
      return less(a, cast<Binary_expr>(b)); 
    }

    bool operator()(Call_expr const* a) const 
    { 
      return less(a, cast<Call_expr>(b)); 
    }

    Expr const* b;
  };


  // If two types are equivalent, then dispatch to
  // an appropriate function.
  return apply(a, Fn(b));
}


bool
less(Constant_expr const* a, Constant_expr const* b)
{
  return a->value() < b->value();
}


bool 
less(Identifier_expr const* a, Identifier_expr const* b)
{
  std::less<String const*> cmp;
  return cmp(a->name(), b->name());
}


bool 
less(Unary_expr const* a, Unary_expr const* b)
{
  if (a->op() < b->op())
    return true;
  if (b->op() < a->op())
    return false;
  return less(a->arg(), b->arg());
}


bool 
less(Binary_expr const* a, Binary_expr const* b)
{
  if (a->op() < b->op())
    return true;
  if (b->op() < a->op())
    return false;
  if (less(a->left(), b->left()))
    return true;
  if (less(b->left(), a->left()))
    return false;
  return less(a->right(), b->right());
}


} // namespace beaker
