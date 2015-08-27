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


template<typename T>
inline typename std::enable_if<is_nullary_node<T>(), bool>::type
less(T const* a, T const * b)
{
  return false;
}


template<typename T>
inline typename std::enable_if<is_unary_node<T>(), bool>::type
less(T const* a, T const * b)
{
  return less(a->first, b->first);
}


template<typename T>
inline typename std::enable_if<is_binary_node<T>(), bool>::type
less(T const* a, T const * b)
{
  if (less(a->first, b->first))
    return true;
  if (less(b->first, a->first))
    return false;
  return less(a->second, b->second);
}


template<typename T>
inline typename std::enable_if<is_ternary_node<T>(), bool>::type
less(T const* a, T const * b)
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
less(std::vector<T const*> a, std::vector<T const*> b)
{
  auto cmp = [](T const* a, T const* b) { return less(a, b); };
  return std::lexicographical_compare(a.begin(), a.end(), 
                                      b.begin(), b.end(), cmp);
}


} // namespace beaker

#endif
