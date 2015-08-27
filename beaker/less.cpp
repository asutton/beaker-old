// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/less.hpp"
#include "beaker/type.hpp"

#include <typeindex>


namespace beaker
{

// Dispatch to an appropriate function.
struct Less_fn
{
  Less_fn(Type const* t)
    : t2(t)
  { }

  bool operator()(Void_type const* t1) const
  { 
    return less(t1, static_cast<Void_type const*>(t2)); 
  }
  
  bool operator()(Boolean_type const* t1) const
  { 
    return less(t1, static_cast<Boolean_type const*>(t2)); 
  }
  
  bool operator()(Integer_type const* t1) const
  { 
    return less(t1, static_cast<Integer_type const*>(t2)); 
  }
  
  bool operator()(Function_type const* t1) const
  { 
    return less(t1, static_cast<Function_type const*>(t2)); 
  }

  bool operator()(Reference_type const* t1) const
  { 
    return less(t1, static_cast<Reference_type const*>(t2)); 
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
  return apply(a, Less_fn(b));
}


} // namespace beaker
