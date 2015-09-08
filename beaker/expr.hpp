// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_EXPR_HPP
#define BEAKER_EXPR_HPP

#include "beaker/prelude.hpp"
#include "beaker/value.hpp"
#include "beaker/operator.hpp"

#include "lingo/node.hpp"


namespace beaker
{

struct Expr_visitor;


// -------------------------------------------------------------------------- //
//                                Expressions

// The Expr class represents the set of all expressions
// in the language.
//
// Every expression has a type. The type of an expression
// is determined by the fules of the language and is known
// at the time an expression is created.
//
// Each expression has a source location, corresponding
// to its position in the source text. The end location
// can be determined programmatically.
struct Expr
{
  Expr(Location l, Type const* t)
    : loc_(l), type_(t)
  { }

  virtual ~Expr() { }

  virtual void accept(Expr_visitor&) const = 0;

  String      node_name() const;
  Location    location() const  { return loc_; }
  Type const* type() const      { return type_; }

  Location    loc_;
  Type const* type_;
};


// The expression visitor.
struct Expr_visitor
{
  virtual void visit(Identifier_expr const*) { }
  virtual void visit(Constant_expr const*) { }
  virtual void visit(Unary_expr const*) { }
  virtual void visit(Binary_expr const*) { }
  virtual void visit(Call_expr const*) { }
};


// An expression that holds a constant value of some type. 
// Values are represented as widest integer type of the host 
// system.
struct Constant_expr : Expr
{
  Constant_expr(Location loc, Type const* t, Value n)
    : Expr(loc, t), value_(n)
  { }

  void accept(Expr_visitor& v) const { v.visit(this); }

  Value value() const { return value_; }

  Value value_;
};


// An id-expression refers to a declaration.
struct Identifier_expr : Expr
{
  Identifier_expr(Location, Decl const*);

  void accept(Expr_visitor& v) const { v.visit(this); }

  String const* name() const;
  Decl const*   decl() const { return decl_; }

  Decl const* decl_;
};


// A unary expression applies an operator to a single argument.
//
// The source location for a unary operation is the location of its 
// operator.
struct Unary_expr : Expr
{
  Unary_expr(Location loc, Type const* t, Unary_op op, Expr const* e)
    : Expr(loc, t), first(op), second(e)
  { }

  void accept(Expr_visitor& v) const { v.visit(this); }

  Unary_op    op() const  { return first; }
  Expr const* arg() const { return second; }

  Unary_op    first;
  Expr const* second;
};


// A helper class for constructing binary expressions. A binary
// expression contains the operator token and its two operands.
//
// Note that the source location of a binary expression is the
// location of its operator.
struct Binary_expr : Expr
{
  Binary_expr(Location loc, Type const* t, Binary_op op, Expr const* l, Expr const* r)
    : Expr(loc, t), first(op), second(l), third(r)
  { }

  void accept(Expr_visitor& v) const { v.visit(this); }

  Binary_op   op() const    { return first; }
  Expr const* left() const  { return second; }
  Expr const* right() const { return third; }

  Binary_op   first;
  Expr const* second;
  Expr const* third;
};


// A function call is comprised of a call target
// and a sequence of arguments.
//
// Note that the target of a function call is 
// represented as an expression, but it must
// refer to a function (declaration or parameter).
struct Call_expr : Expr
{
  Call_expr(Location loc, Type const* t, Expr const* f, Expr_seq const& a)
    : Expr(loc, t), first(f), second(a)
  { }

  void accept(Expr_visitor& v) const { v.visit(this); }

  Expr const*     function() const  { return first; }
  Expr_seq const& arguments() const { return second; }

  Expr const* first;
  Expr_seq    second;
};



// -------------------------------------------------------------------------- //
//                            Expression builders
//
// These functions create new expressions from their arguments. 
// To the greatest extent possible, these functions attempt to 
// resolve the type of the expression from those arguments.


Constant_expr*    make_bool_expr(Location, bool);
Constant_expr*    make_int_expr(Location, Value);
Constant_expr*    make_value_expr(Location, Type const*, Value);
Identifier_expr*  make_identifier_expr(Location, Decl const*);
Unary_expr*       make_unary_expr(Location, Unary_op, Expr const*);
Binary_expr*      make_binary_expr(Location, Binary_op, Expr const*, Expr const*);
Call_expr*        make_call_expr(Location, Expr const*, Expr_seq const&);


// Returns the boolean literal `true`.
inline Constant_expr*
make_bool_expr(bool b)
{
  return make_bool_expr({}, b);
}


// Returns the boolean literal `true`.
inline Constant_expr*
make_true_expr()
{
  return make_bool_expr(true);
}


// Returns the boolean literal `false`.
inline Constant_expr*
make_false_expr()
{
  return make_bool_expr(false);
}


// Returns an integer literal expression.
inline Constant_expr*
make_int_expr(Value n)
{
  return make_int_expr({}, n);
}


// Returns an value expression having the given type.
inline Constant_expr*
make_constant_expr(Type const* t, Value n)
{
  return make_value_expr({}, t, n);
}


// Returns a identifier-expression for the declaration `d`.
inline Identifier_expr*
make_identifier_expr(Decl const* d)
{
  return make_identifier_expr({}, d);
}


// Returns a unary expression.
inline Unary_expr*
make_unary_expr(Unary_op op, Expr const* e)
{
  return make_unary_expr({}, op, e);
}


// Returns a binary expression.
inline Binary_expr*
make_binary_expr(Binary_op op, Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, op, e1, e2);
}


#if 0

// Make the expression `e1 + e2`.
inline Binary_expr*
make_add_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, num_add_op, e1, e2);
}


// Make the expression `e1 - e2`.
inline Binary_expr*
make_sub_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, num_sub_op, e1, e2);
}


// Make the expression `e1 * e2`.
inline Binary_expr*
make_mul_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, num_mul_op, e1, e2);
}


// Make the expression `e1 / e2`.
inline Binary_expr*
make_div_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, num_div_op, e1, e2);
}


// Make the expression `e1 % e2`.
inline Binary_expr*
make_mod_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, num_mod_op, e1, e2);
}


// Make the expression `-e`.
inline Unary_expr*
make_neg_expr(Expr const* e)
{
  return make_unary_expr({}, num_neg_op, e);
}


// Make the expression '`+e`
inline Unary_expr*
make_pos_expr(Expr const* e)
{
  return make_unary_expr({}, num_pos_op, e);
}


// Bitwise expressions.

inline Binary_expr*
make_bitwise_and_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, bit_and_op, e1, e2);
}


inline Binary_expr*
make_bitwise_or_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, bit_or_op, e1, e2);
}


inline Binary_expr*
make_bitwise_xor_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, bit_xor_op, e1, e2);
}


inline Unary_expr*
make_bitwise_not_expr(Expr const* e)
{
  return make_unary_expr({}, bit_not_op, e);
}


inline Binary_expr*
make_bitwise_lsh_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, bit_lsh_op, e1, e2);
}


inline Binary_expr*
make_bitwise_rsh_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, bit_rsh_op, e1, e2);
}


// Relational expressions

// Make the expression `e1 == e2`.
inline Binary_expr*
make_eq_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, rel_eq_op, e1, e2);
}


inline Binary_expr*
make_ne_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, rel_ne_op, e1, e2);
}


inline Binary_expr*
make_lt_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, rel_lt_op, e1, e2);
}


inline Binary_expr*
make_gt_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, rel_gt_op, e1, e2);
}


inline Binary_expr*
make_le_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, rel_le_op, e1, e2);
}


inline Binary_expr*
make_ge_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, rel_ge_op, e1, e2);
}


// Logical expressions

// Make the expression `e1 && e2`.
inline Binary_expr*
make_logical_and_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, log_and_op, e1, e2);
}


// Make the expression `e1 || e2`.
inline Binary_expr*
make_logical_or_expr(Expr const* e1, Expr const* e2)
{
  return make_binary_expr({}, log_or_op, e1, e2);
}


// Make the expression `!e`.
inline Unary_expr*
make_logical_not_expr(Expr const* e)
{
  return make_unary_expr({}, log_not_op, e);
}

// TODO: Finish building out constructors for these 
// expressions.

#endif


inline Call_expr*
make_call_expr(Expr const* f, Expr_seq const& a)
{
  return make_call_expr({}, f, a);
}


// -------------------------------------------------------------------------- //
//                            Queries

bool has_boolean_type(Expr const*);
bool has_integer_type(Expr const*);


// -------------------------------------------------------------------------- //
//                            Generic visitor


// A parameterized visitor that dispatches to a function object.
template<typename F, typename T>
struct Generic_expr_visitor : Expr_visitor, Generic_visitor<F, T>
{
  Generic_expr_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Constant_expr const* e) { this->invoke(e); }
  void visit(Identifier_expr const* e) { this->invoke(e); }
  void visit(Unary_expr const* e) { this->invoke(e); }
  void visit(Binary_expr const* e) { this->invoke(e); }
  void visit(Call_expr const* e) { this->invoke(e); }
};


// Apply the function f to the expression e.
template<typename F, typename T = typename std::result_of<F(Constant_expr*)>::type>
inline T
apply(Expr const* e, F fn)
{
  Generic_expr_visitor<F, T> v(fn);
  return accept(e, v);
}


} // namespace beaker

#endif
