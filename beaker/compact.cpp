// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/compact.hpp"
#include "beaker/expr.hpp"
#include "beaker/less.hpp"

#include <set>


namespace beaker
{

namespace
{

// FIXME: Use a hash set. Of course that means we have
// to define a hash function.
using Leaf_set = std::set<Expr const*, Term_less>;


Expr const* compact(Leaf_set&, Expr const*);


// Insert the constant expression into the leaf set.
Expr const*
compact(Leaf_set& leaf, Constant_expr const* e)
{
  return *leaf.insert(e).first;
}


// Insert the identifier expression into the leaf set.
Expr const*
compact(Leaf_set& leaf, Identifier_expr const* e)
{
  return *leaf.insert(e).first;
}


// FIXME: I should be able to explicitly specify the types
// for these expression constructors. Otherwise, we're going
// to do a bunch of redundant checking when we compact an
// expression.


// Build a new unary expression.
Expr const*
compact(Leaf_set& leaf, Unary_expr const* e)
{
  Expr const* e1 = compact(e->arg());
  return make_unary_expr(e->op(), e1);
}


// Build a new binary expression.
Expr const*
compact(Leaf_set& leaf, Binary_expr const* e)
{
  Expr const* e1 = compact(leaf, e->left());
  Expr const* e2 = compact(leaf, e->right());
  return make_binary_expr(e->op(), e1, e2);
}


Expr const*
compact(Leaf_set& leaf, Call_expr const* e)
{
  Expr const* f = compact(leaf, e->function());

  Expr_seq args;
  args.reserve(e->arguments().size());
  for (Expr const* ei : e->arguments())
    args.push_back(compact(leaf, ei));

  return make_call_expr(f, args);
}


// Compat an expression.
Expr const*
compact(Leaf_set& leaf, Expr const* e)
{
  struct Fn
  {
    Fn(Leaf_set& l)
      : l(l)
    { }

    Expr const* operator()(Constant_expr const* e) const { return compact(l, e); }
    Expr const* operator()(Identifier_expr const* e) const { return compact(l, e); }
    Expr const* operator()(Unary_expr const* e) const { return compact(l, e); }
    Expr const* operator()(Binary_expr const* e) const { return compact(l, e); }
    Expr const* operator()(Call_expr const* e) const { return compact(l, e); }

    Leaf_set& l;
  };

  return apply(e, Fn(leaf));
}


} // namespace


Expr const*
compact(Expr const* e)
{
  Leaf_set leaf;
  return compact(leaf, e);
}

} // namespace beaker
