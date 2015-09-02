// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_PRELUDE_HPP
#define BEAKER_PRELUDE_HPP

// This module provides declarations used in ever module
// in the implementation. It should be the first include
// in each header file.

// Strings
#include "lingo/string.hpp"

// Arbitrary precision integers.
#include "lingo/integer.hpp"

// Runtime checks and diagnostics.
#include "lingo/error.hpp"

// AST concepts and operations.
#include "lingo/node.hpp"

// Memory facilities.
#include "lingo/memory.hpp"

// Pretty printing.
#include "lingo/print.hpp"

// Compiler debugging.
#include "lingo/debug.hpp"

// Support sequences of terms.
#include <vector>


namespace beaker
{

// Lingo imports
using namespace lingo;

// Overloaded functions
using lingo::is;
using lingo::as;
using lingo::cast;
using lingo::print;
using lingo::debug;
using lingo::mark;


// Steve terms
struct Type;
struct Void_type;
struct Boolean_type;
struct Integer_type;
struct Function_type;
struct Reference_type;

struct Expr;
struct Constant_expr;
struct Identifier_expr;
struct Unary_expr;
struct Binary_expr;
struct Call_expr;

struct Decl;
struct Variable_decl;
struct Function_decl;
struct Parameter_decl;

struct Stmt;
struct Empty_stmt;
struct Declaration_stmt;
struct Expression_stmt;
struct Assignment_stmt;
struct If_then_stmt;
struct If_else_stmt;
struct While_stmt;
struct Do_stmt;
struct Return_stmt;
struct Block_stmt;
struct File_stmt;


// A sequence of types.
using Type_seq = std::vector<Type const*>;


// A sequence of expressions.
using Expr_seq = std::vector<Expr const*>;


// A sequence of declarations.
using Decl_seq = std::vector<Decl const*>;


// A list of statements.
using Stmt_seq = std::vector<Stmt const*>;


} // namespace beaker


// Include pretty printing by default.
#include "beaker/print.hpp"


#endif

