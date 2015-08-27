// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_SAME_HPP
#define BEAKER_SAME_HPP

// This module defines the equivalence of terms.

#include "beaker/prelude.hpp"

#include <algorithm>


namespace beaker
{

bool same(Type const*, Type const*);


// Two nullary terms are equivalent. Note that many
// nullary nodes may have other properties.
template<typename T>
inline typename std::enable_if<is_nullary_node<T>(), bool>::type
same(T const* a, T const * b)
{
  return true;
}


template<typename T>
inline typename std::enable_if<is_unary_node<T>(), bool>::type
same(T const* a, T const * b)
{
  return same(a->first, b->first);
}


template<typename T>
inline typename std::enable_if<is_binary_node<T>(), bool>::type
same(T const* a, T const * b)
{
  return same(a->first, b->first) && same(a->second, b->second);
}


template<typename T>
inline typename std::enable_if<is_ternary_node<T>(), bool>::type
same(T const* a, T const * b)
{
  return same(a->first, b->first) 
      && same(a->second, b->second)
      && same(a->third, b->third);
}


// Lexicographically compares sequences of terms.
template<typename T>
inline bool
same(std::vector<T const*> a, std::vector<T const*> b)
{
  auto cmp = [](T const* a, T const* b) { return same(a, b); };
  return std::equal(a.begin(), a.end(), b.begin(), cmp);
}


} // namespace beaker

#endif
