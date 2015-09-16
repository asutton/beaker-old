// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_LESS_HPP
#define BEAKER_LESS_HPP

// This module defines a lexicographical ordering over the
// terms of a language.
//
// TODO: This is begging to be made generic. There are only
// a handful implementations: arity overloads and sequences.

#include "beaker/prelude.hpp"

#include <algorithm>


namespace beaker
{

bool less(Type const*, Type const*);

bool less(Expr const*, Expr const*);
bool less(Constant_expr const*, Constant_expr const*);
bool less(Identifier_expr const*, Identifier_expr const*);
bool less(Unary_expr const*, Unary_expr const*);
bool less(Binary_expr const*, Binary_expr const*);


template<typename T>
inline typename std::enable_if<is_nullary_node<T>(), bool>::type
less(T const* a, T const* b)
{
  return false;
}


template<typename T>
inline typename std::enable_if<is_unary_node<T>(), bool>::type
less(T const* a, T const* b)
{
  return less(a->first, b->first);
}


template<typename T>
inline typename std::enable_if<is_binary_node<T>(), bool>::type
less(T const* a, T const* b)
{
  if (less(a->first, b->first))
    return true;
  if (less(b->first, a->first))
    return false;
  return less(a->second, b->second);
}


template<typename T>
inline typename std::enable_if<is_ternary_node<T>(), bool>::type
less(T const* a, T const* b)
{
  if (less(a->first, b->first))
    return true;
  if (less(b->first, a->first))
    return false;
  if (less(a->second, b->second))
    return true;
  if (less(b->second, a->second))
    return false;
  return less(a->third, b->third);
}


// Lexicographically compares sequences of terms.
template<typename T>
inline bool
less(std::vector<T const*> const& a, std::vector<T const*> const& b)
{
  auto cmp = [](T const* a, T const* b) { return less(a, b); };
  return std::lexicographical_compare(a.begin(), a.end(), 
                                      b.begin(), b.end(), cmp);
}


// A comparator for terms.
struct Term_less
{
  template<typename T, typename U>
  bool operator()(T const& a, U const& b) const
  {
    return less(a, b);
  }
};


} // namespace beaker

#endif
