// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/function.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/same.hpp"


namespace beaker
{

// -------------------------------------------------------------------------- //
//                            Return type checking

#if 0
namespace
{


Type const* check(Stmt const*, Type const*);


// Check that any return statements in this block have the
// same type as `t`. Returns the last seen return type in
// block, or nullptr if no such returns exist.
Type const*
check(Block_stmt const* s, Type const* t)
{
  Type const* t0 = nullptr;

  for (Stmt const* s0 : *s) {
    // Check the nested statement. If we got a non-null
    // response then the statement was either a return
    // or a compound statement containing a return. Note
    // that the return could have been an error.
    //
    // In general, save the last such result. However,
    // if we had previously gotten an error, don't update
    // the most recent return value.
    if (Type const* t1 = check(s0, t))
      if (!is_error_node(t0))
        t0 = t1;
  }

  return t0;
}


// The semantics are similar to a block statement. Look through 
// all statementsand determine the return type for each one.
Type const*
check(Match_stmt const* s, Type const* t)
{
  Type const* t0 = nullptr;

  for (Stmt const* s0 : s->cases()) {
    if (Type const* t1 = check(s0, t))
      if (!is_error_node(t0))
        t0 = t1;
  }

  return t0;
}


// Check the return type for the statement of the of
// case label.
Type const*
check(Case_stmt const* s, Type const* t)
{
  return check(s->stmt(), t);
}


// Check that the type of the result type is the same
// as `t`.
//
// TODO: Weaken to allow conversions.
Type const*
check(Return_stmt const* s, Type const* t)
{
  Expr const* e = s->result();
  if (e->type() != t) {
    if (t == get_void_type()) {
      error(e->location(), "returning a value from a 'void' function");
      return make_error_node<Type>();
    }

    // Otherwise, peform conversion. Note that we have
    // to update the expression of the return statement
    // with the converted operand.
    //
    // NOTE: We could allow the `void` issue above to
    // fail in conversion (since values don't convert to
    // void), but the diagnostic above would be more
    // appropriate.
    //
    // TODO: This is kind of gross (but not entirely without
    // precedent -- we retroactively adjust the type of
    // enumerators). But it would be more elegant if the
    // creation of the return statement was done in a
    // context where the return type was available. But
    // we would need to define what that was.

    Expr const* c = convert(e, t);
    if (!c)
      return make_error_node<Type>();
    
    // TODO: Yuck.
    Return_stmt* s1 = const_cast<Return_stmt*>(s);
    s1->first = c;
    return t;
  }
  return t;
}

struct Return_type_fn
{

};

// Search for returns of the given statement and ensure
// that they have type `t`.
//
// TODO: We could also, at this point, ensure that each path 
// returns a value. Or we could do that later...
Type const*
check(Stmt const* s, Type const* t)
{
  lingo_assert(is_valid_node(s));
  switch (s->kind()) {
    case block_stmt:
      return check(cast<Block_stmt>(s), t);

    case return_stmt: 
      return check(cast<Return_stmt>(s), t);

    case match_stmt:
      return check(cast<Match_stmt>(s), t);

    case case_stmt:
      return check(cast<Case_stmt>(s), t);

    case empty_stmt:
    case expr_stmt:
    case decl_stmt:
    case do_stmt:
    case instruct_stmt:
      // These do not indicate return types.
      return nullptr;
  }
  lingo_unreachable("unhandled statement '{}'", s->node_name());
}


} // namespace
#endif


// Check that the return statements in `s` match the function
// return type `t`.
bool
check_return_type(Type const* t, Stmt const* s)
{
  // // Check for/find a return type.
  // Type const* r = check_return_type(s, t);
  // if (is_error_node(r))
  //   return false;

  // // If no return type was found, then the return type
  // // of the function must be void.
  // //
  // // FIXME: Source location? Also, a more appropriate error
  // // would be something like, "not all paths return a value"
  // // or something like that.
  // if (!r && t != get_void_type()) {
  //   error("no return statement in non-void function");
  //   return false;
  // }

  return true;
}

} // namespace beaker