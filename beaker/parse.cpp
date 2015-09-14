// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/parse.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/decl.hpp"
#include "beaker/stmt.hpp"
#include "beaker/unit.hpp"
#include "beaker/operator.hpp"
#include "beaker/variable.hpp"
#include "beaker/function.hpp"
#include "beaker/lookup.hpp"
#include "beaker/evaluate.hpp"


namespace beaker
{

Decl const* parse_decl(Parser&, Token_stream&);


// Parse a translation unit.
//
//    unit ::= decl-seq
//
Unit const*
parse_file(Parser& p, Token_stream& ts)
{
  // Enter the global lexical scope.
  Global_scope scope;

  using Term = Sequence_term<Decl>;
  if (Required<Term> enc = parse_sequence(p, ts, parse_decl))
    return p.on_unit(**enc);
  else
    return make_error_node<Unit>();
}

Unit const*
parse(Token_list const& toks)
{
  // Note that the token stream will update the input
  // context as we consume tokens.
  Token_stream ts(toks);  
  Parser p;
  return parse_file(p, ts);
}


// -------------------------------------------------------------------------- //
//                              Type semantics


Type const*
Parser::on_void_type(Token const*)
{
  return get_void_type();
}


Type const*
Parser::on_bool_type(Token const*)
{
  return get_bool_type();
}


Type const*
Parser::on_int_type(Token const*)
{
  return get_int_type();
}


// -------------------------------------------------------------------------- //
//                            Expression semantics

Expr const*
Parser::on_boolean_lit(Token const* tok)
{
  return make_bool_expr(tok->location(), as_bool(*tok));
}


Expr const*
Parser::on_integer_lit(Token const* tok)
{
  return make_int_expr(tok->location(), as_int(*tok));
}


Expr const*
Parser::on_identifier_expr(Token const* tok)
{
  if (Required<Decl> decl = lookup(tok->str()))
    return make_identifier_expr(tok->location(), *decl);
  error(tok->location(), "unresolved symbol '{}'", *tok);
  return make_error_node<Expr>();
}



Expr const*
Parser::on_call_expr(Token const* tok, Expr const* fn, Expr_seq const& args)
{
  return make_call_expr(tok->location(), fn, args);
}


Expr const*
Parser::on_unary_expr(Token const* tok, Expr const* e)
{
  Location loc = tok->location();
  switch (tok->kind()) {
  case plus_tok: 
    return make_unary_expr(loc, num_pos_op, e);
  case minus_tok: 
    return make_unary_expr(loc, num_neg_op, e);
  case tilde_tok: 
    return make_unary_expr(loc, bit_not_op, e);
  case bang_tok: 
    return make_unary_expr(loc, log_not_op, e);
  default:
    break;
  }
  lingo_unreachable();
}


Expr const*
Parser::on_binary_expr(Token const* tok, Expr const* e1, Expr const* e2)
{
  Location loc = tok->location();
  switch (tok->kind()) {
  case plus_tok: 
    return make_binary_expr(loc, num_add_op, e1, e2);
  case minus_tok: 
    return make_binary_expr(loc, num_sub_op, e1, e2);
  case star_tok: 
    return make_binary_expr(loc, num_mul_op, e1, e2);
  case slash_tok: 
    return make_binary_expr(loc, num_div_op, e1, e2);
  case percent_tok: 
    return make_binary_expr(loc, num_mod_op, e1, e2);
  case eq_eq_tok: 
    return make_binary_expr(loc, rel_eq_op, e1, e2);
  case bang_eq_tok: 
    return make_binary_expr(loc, rel_ne_op, e1, e2);
  case lt_tok: 
    return make_binary_expr(loc, rel_lt_op, e1, e2);
  case gt_tok: 
    return make_binary_expr(loc, rel_gt_op, e1, e2);
  case lt_eq_tok: 
    return make_binary_expr(loc, rel_le_op, e1, e2);
  case gt_eq_tok: 
    return make_binary_expr(loc, rel_ge_op, e1, e2);
  case amp_tok: 
    return make_binary_expr(loc, bit_and_op, e1, e2);
  case caret_tok: 
    return make_binary_expr(loc, bit_xor_op, e1, e2);
  case bar_tok: 
    return make_binary_expr(loc, bit_or_op, e1, e2);
  case lt_lt_tok: 
    return make_binary_expr(loc, bit_lsh_op, e1, e2);
  case gt_gt_tok: 
    return make_binary_expr(loc, bit_rsh_op, e1, e2);
  case amp_amp_tok: 
    return make_binary_expr(loc, log_and_op, e1, e2);
  case bar_bar_tok: 
    return make_binary_expr(loc, log_or_op, e1, e2);
  default:
    break;
  }
  lingo_unreachable();
}



// Handle a variable declaration. Create an uninitialized variable
// and declare it in the current scope.
Decl const*
Parser::on_variable_decl(Token const* tok, Token const* id, Type const* t)
{
  Decl const* d = make_variable_decl(tok->location(), id->str(), t);
  if (!declare(d))
    return make_error_node<Decl>();
  return d;
}


// Handle a variable initializer. Check that the initializer matches
// the declared type of the variable.
//
// The initializer is reduced at this point.
Decl const*
Parser::on_variable_init(Decl const* d, Expr const* e)
{
  Variable_decl const* v = cast<Variable_decl>(d);
  if (check_initializer(v, e)) {
    // Reduce the initializer.
    Expr const* r = reduce(e);
    
    // Update the variable with the reduced initializer.
    modify(v)->initialize(r);
    return v;
  }
  return make_error_node<Decl>();
}


// Handle a function declaration. Creates an undefined function
// and declares it in the current scope.
Decl const*
Parser::on_function_decl(Token const* tok, Token const* id, Decl_seq const& parms, Type const* t)
{
  Decl const* d = make_function_decl(tok->location(), id->str(), parms, t);
  if (!declare(d))
    return make_error_node<Decl>();
  return d;
}


// Start the function definition by declaring all parameters. 
Decl const*
Parser::on_function_start(Decl const* d)
{
  Function_decl const* f = cast<Function_decl>(d);
  for (Decl const* p : f->parameters()) {
    if (!declare(p))
      d = make_error_node<Decl>();
  }
  return d;
}


// Finish the function definitionby assigning the statement.
Decl const*
Parser::on_function_finish(Decl const* d, Stmt const* s)
{
  Function_decl const* f = cast<Function_decl>(d);
  if (check_definition(f, s)) {
    modify(f)->define(s);
    return f;
  }
  return make_error_node<Decl>();
}


Decl const*
Parser::on_parameter_decl(Token const* id, Type const* t)
{
  return make_parameter_decl(id->location(), id->str(), t);
}


Stmt const*
Parser::on_empty_stmt(Token const* tok)
{
  return make_empty_stmt(tok->location());
}


Stmt const*
Parser::on_block_stmt(Token const* l, Token const* r, Stmt_seq const& s)
{
  return make_block_stmt(l->location(), r->location(), s);
}


Stmt const*
Parser::on_declaration_stmt(Decl const* d)
{
  return make_declaration_stmt(d);
}


Stmt const*
Parser::on_if_then_stmt(Token const* tok, Expr const* e, Stmt const* b)
{
  return make_if_then_stmt(tok->location(), e, b);
}


Stmt const*
Parser::on_if_else_stmt(Token const* tok1, Token const* tok2, Expr const* e, Stmt const* b1, Stmt const* b2)
{
  return make_if_else_stmt(tok1->location(), tok2->location(), e, b1, b2);
}


Stmt const*
Parser::on_while_stmt(Token const*, Expr const*, Stmt const*)
{
  return nullptr;
}


Stmt const*
Parser::on_do_stmt(Token const*, Token const*, Expr const*, Stmt const*)
{
  return nullptr;
}


Stmt const*
Parser::on_return_stmt(Token const* tok1, Token const* tok2, Expr const* e)
{
  return make_return_stmt(tok1->location(), tok2->location(), e);
}


Stmt const*
Parser::on_expression_stmt(Token const* tok, Expr const* e)
{
  return make_expression_stmt(tok->location(), e);
}


Stmt const*
Parser::on_assignment_stmt(Token const* tok1, Token const* tok2, Expr const* e1, Expr const* e2)
{
  // FIXME: Check that e1 is a reference type.
  // return make_assignment_stmt(tok1->location(), tok2->location(), e1, e2);
  return nullptr;
}



// Simply construct a new node comprising those statements.
Unit const*
Parser::on_unit(Decl_seq const& d)
{
  return make_unit(d);
}


// TODO: Install the grammar rule/name associations.
void
init_grammar()
{
  
}


} // namespace beaker

