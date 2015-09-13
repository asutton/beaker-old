// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/print.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/decl.hpp"
#include "beaker/stmt.hpp"
#include "beaker/unit.hpp"


namespace beaker
{

struct Print_fn
{
  Print_fn(Printer& p)
    : p(p)
  { }

  void operator()(Void_type const* t) const { print(p, t); }
  void operator()(Boolean_type const* t) const { print(p, t); }
  void operator()(Integer_type const* t) const { print(p, t); }
  void operator()(Function_type const* t) const { print(p, t); }
  void operator()(Reference_type const* t) const { print(p, t); }

  void operator()(Constant_expr const* e) const { print(p, e); }
  void operator()(Identifier_expr const* e) const { print(p, e); }
  void operator()(Unary_expr const* e) const { print(p, e); }
  void operator()(Binary_expr const* e) const { print(p, e); }
  void operator()(Call_expr const* e) const { print(p, e); }

  void operator()(Variable_decl const* d) const { print(p, d); }
  void operator()(Function_decl const* d) const { print(p, d); }
  void operator()(Parameter_decl const* d) const { print(p, d); }

  void operator()(Empty_stmt const* s) { print(p, s); }
  void operator()(Declaration_stmt const* s) { print(p, s); }
  void operator()(Expression_stmt const* s) { print(p, s); }
  void operator()(Assignment_stmt const* s) { print(p, s); }
  void operator()(If_then_stmt const* s) { print(p, s); }
  void operator()(If_else_stmt const* s) { print(p, s); }
  void operator()(While_stmt const* s) { print(p, s); }
  void operator()(Do_stmt const* s) { print(p, s); }
  void operator()(Return_stmt const* s) { print(p, s); }
  void operator()(Block_stmt const* s) { print(p, s); }

  Printer& p;
};


// -------------------------------------------------------------------------- //
//                                    Types

// Print a type.
//
//    type ::= <see grammar>
void 
print(Printer& p, Type const* t)
{
  return apply(t, Print_fn(p));
}


// Print the void type.
//
//    void-type ::= 'bool'
void
print(Printer& p, Void_type const*)
{
  print(p, "void");
}


// Print the boolean type.
//
//    boolean-type ::= 'bool'
void
print(Printer& p, Boolean_type const*)
{
  print(p, "bool");
}


// Print the integer type.
//
//    integer-type ::= 'int'
void
print(Printer& p, Integer_type const*)
{
  print(p, "int");
}


// Print a function type.
//
//    function-type ::= '(' parameter-type-list ')' '->' type
void
print(Printer& p, Function_type const* t)
{
  print(p, '(');
  print_list(p, t->parameter_types());
  print(p, ')');
  print(p, " -> ");
  print(p, t->return_type());
}


// Print a reference type.
//
//    reference-type ::= 'ref' type
void
print(Printer& p, Reference_type const* t)
{
  print(p, "ref ");
  print(p, t->type());
}


// -------------------------------------------------------------------------- //
//                                  Expressions

// Print an expression.
//
//    expression ::= <see grammar>
void 
print(Printer& p, Expr const* e)
{
  return apply(e, Print_fn(p));
}


// Print a value expression.
//
//    value-expression ::= literal
//
// The printed value depends on the type of the expression.
void
print(Printer& p, Constant_expr const* e)
{
  if (e->type() == get_bool_type())
    print(p, e->value() ? "true" : "false");
  else
    print(p, e->value());
}


// Print an identifier-expression. This just prints
// the identifier.
//
//    identifier-expression ::= identifier
void
print(Printer& p, Identifier_expr const* e)
{
  print(p, e->name());
}


// Print a unary expression.
//
//    unary-expression ::=
//        '+' postfix-expression
//      | '-' postfix-expression
//      | '~' postfix-expression
//      | '!' postfix-expression
//
// TODO: Print parens if needed.
void
print(Printer& p, Unary_expr const* e)
{
  print(p, e->op());
  print(p, e->arg());
}


// Print a binary expression.
//
//    binary-expression ::=
//        ...
//
// TODO: Print parens if needed.
void
print(Printer& p, Binary_expr const* e)
{
  print(p, e->left());
  print_space(p);
  print(p, e->op());
  print_space(p);
  print(p, e->right());
}


// Print a call expression.
//
//    call-expression ::= postfix-expression '(' argument-list ')'
void
print(Printer& p, Call_expr const* e)
{
  // TODO: Apply parens to the function argument?
  print(p, e->function());
  print(p, '(');
  print_list(p, e->arguments());
  print(p, ')');
}


// -------------------------------------------------------------------------- //
//                                  Declarations

void
print(Printer& p, Decl const* d)
{
  return apply(d, Print_fn(p));
}


void
print(Printer& p, Variable_decl const* d)
{
  print(p, "var ");
  print(p, d->name());
  print(p, " : ");
  print(p, d->type());
  print(p, " = ");
  print(p, d->initializer());
  print(p, ';');
}


void
print(Printer& p, Function_decl const* d)
{
  print(p, "def ");
  print(p, d->name());
  print(p, "(...)"); // FIXME: Actually print parens.
  print(p, " -> ");
  print(p, d->return_type());
  print_space(p);
  print(p, d->body());
}


void
print(Printer& p, Parameter_decl const* d)
{
  print(p, d->name());
  print(p, " : ");
  print(p, d->type());
}


// -------------------------------------------------------------------------- //
//                                  Statements

void
print(Printer& p, Stmt const* s)
{
  apply(s, Print_fn(p));
}


void
print(Printer& p, Empty_stmt const* s)
{
  print(p, ';');
}


void
print(Printer& p, Declaration_stmt const* s)
{
  print(p, s->decl());
}


void
print(Printer& p, Expression_stmt const* s)
{
  print(p, s->expr());
}


void
print(Printer& p, Assignment_stmt const* s)
{
  print(p, s->lhs());
  print(p, " = ");
  print(p, s->rhs());
}


namespace
{

void
print_paren_expr(Printer& p, Expr const* e)
{
  print(p, '(');
  print(p, e);
  print(p, ')');
}

} // namespace


void
print(Printer& p, If_then_stmt const* s)
{
  print(p, "if ");
  print_paren_expr(p, s->condition());
  print_space(p);
  print(p, s->branch());
}


void
print(Printer& p, If_else_stmt const* s)
{
  print(p, "if/else; ");

}


void
print(Printer& p, While_stmt const* s)
{
  print(p, "while;");
}


void
print(Printer& p, Do_stmt const* s)
{
  print(p, "do;");
}


void
print(Printer& p, Return_stmt const* s)
{
  print(p, "return ");
  print(p, s->result());
  print(p, ';');
}


void
print(Printer& p, Block_stmt const* s)
{
  print(p, '{');
  print_nested(p, s->statements());
  print(p, '}');
}


// -------------------------------------------------------------------------- //
//                             Translation unit

void
print(Printer& p, Unit const* u)
{
  for (Decl const* d : u->declarations()) {
    print(p, d);
    print_newline(p);
  }
}





} // namespace beaker
