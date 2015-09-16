// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/graph.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/decl.hpp"
#include "beaker/stmt.hpp"
#include "beaker/unit.hpp"


namespace beaker
{

namespace
{

using Id_map = std::unordered_map<Expr const*, int>;

int
put_node(Id_map& id, Expr const* e) 
{
  auto x = id.insert({e, 0});
  if (x.second)
    x.first->second = id.size();
  return x.first->second;
}


int
get_node(Id_map const& id, Expr const* e)
{
  auto iter = id.find(e);
  return iter->second;
}


// -------------------------------------------------------------------------- //
//                            Node names
//
// Generate a textual label for a node.

String node_label(Id_map&, Expr const*);


String 
node_label(Id_map& id, Constant_expr const* e)
{
  return format("{}", e->value());
}


String 
node_label(Id_map& id, Identifier_expr const* e)
{
  return *e->name();
}


String 
node_label(Id_map& id, Unary_expr const* e)
{
  return get_spelling(e->op());
}


String 
node_label(Id_map& id, Binary_expr const* e)
{
  return get_spelling(e->op());
}


String 
node_label(Id_map& id, Call_expr const* e)
{
  return "call";
}


String
node_label(Id_map& id, Expr const* e)
{
  struct Fn
  {
    Fn(Id_map& m)
      : id(m)
    { }

    String operator()(Constant_expr const* e) const { return node_label(id, e); }
    String operator()(Identifier_expr const* e) const { return node_label(id, e); }
    String operator()(Unary_expr const* e) const { return node_label(id, e); }
    String operator()(Binary_expr const* e) const { return node_label(id, e); }
    String operator()(Call_expr const* e) const { return node_label(id, e); }

    Id_map& id;
  };

  return apply(e, Fn(id));
}


String 
node_name(Id_map& id, Expr const* e)
{
  return format("n{}", get_node(id, e));
}


// -------------------------------------------------------------------------- //
//                            List nodes

template<typename T>
void
list_node_common(Printer& p, Id_map& id, T const* e)
{
  int n = put_node(id, e);
  print(p, "n{} [label=\"{}\"]", n, node_label(id, e));
  print_newline(p);
}


void list_nodes(Printer&, Id_map&, Expr const*);


void
list_nodes(Printer& p, Id_map& id, Constant_expr const* e)
{
  list_node_common(p, id, e);
}


void
list_nodes(Printer& p, Id_map& id, Identifier_expr const* e)
{
  list_node_common(p, id, e);
}


void
list_nodes(Printer& p, Id_map& id, Unary_expr const* e)
{
  list_node_common(p, id, e);
  list_nodes(p, id, e->arg());
}


void
list_nodes(Printer& p, Id_map& id, Binary_expr const* e)
{
  list_node_common(p, id, e);
  list_nodes(p, id, e->left());
  list_nodes(p, id, e->right());
}


void
list_nodes(Printer& p, Id_map& id, Call_expr const* e)
{
  list_node_common(p, id, e);
  for (Expr const* ei : e->arguments())
    list_nodes(p, id, ei);
}


void 
list_nodes(Printer& p, Id_map& id, Expr const* e)
{
  struct Fn
  {
    Fn(Printer& p, Id_map& m)
      : p(p), id(m)
    { }

    void operator()(Constant_expr const* e) const { list_nodes(p, id, e); }
    void operator()(Identifier_expr const* e) const { list_nodes(p, id, e); }
    void operator()(Unary_expr const* e) const { list_nodes(p, id, e); }
    void operator()(Binary_expr const* e) const { list_nodes(p, id, e); }
    void operator()(Call_expr const* e) const { list_nodes(p, id, e); }

    Printer& p;
    Id_map& id;
  };

  return apply(e, Fn(p, id));
}

// -------------------------------------------------------------------------- //
//                            List nodes

void list_arrows(Printer&, Id_map&, Expr const*);


void 
list_arrows(Printer& p, Id_map& id, Constant_expr const* e)
{
}


void 
list_arrows(Printer& p, Id_map& id, Identifier_expr const* e)
{
}


void 
list_arrows(Printer& p, Id_map& id, Unary_expr const* e)
{
  list_arrows(p, id, e->arg());

  print(p, "{} -> {};", node_name(id, e), node_name(id, e->arg()));
  print_newline(p);
}


void 
list_arrows(Printer& p, Id_map& id, Binary_expr const* e)
{
  list_arrows(p, id, e->left());
  list_arrows(p, id, e->right());

  String src = node_name(id, e);
  print(p, "{} -> {};", src, node_name(id, e->left()));
  print_newline(p);
  print(p, "{} -> {};", src, node_name(id, e->right()));
  print_newline(p);
}


void 
list_arrows(Printer& p, Id_map& id, Call_expr const* e)
{
  put_node(id, e);
  for (Expr const* ei : e->arguments())
    list_arrows(p, id, ei);


  String src = node_name(id, e);
  for (Expr const* ei : e->arguments()) {
    print(p, "{} -> {};", src, node_name(id, ei));
    print_newline(p);
  }
}


void 
list_arrows(Printer& p, Id_map& id, Expr const* e)
{
  struct Fn
  {
    Fn(Printer& p, Id_map& m)
      : p(p), id(m)
    { }

    void operator()(Constant_expr const* e) const { list_arrows(p, id, e); }
    void operator()(Identifier_expr const* e) const { list_arrows(p, id, e); }
    void operator()(Unary_expr const* e) const { list_arrows(p, id, e); }
    void operator()(Binary_expr const* e) const { list_arrows(p, id, e); }
    void operator()(Call_expr const* e) const { list_arrows(p, id, e); }

    Printer& p;
    Id_map& id;
  };

  return apply(e, Fn(p, id));
}


} // namespace


void
graph(Expr const* e)
{
  Printer p(std::cerr);
  Id_map id; 

  print(p, "digraph G {\n");
  list_nodes(p, id, e);
  list_arrows(p, id, e);
  print(p, "}\n");
}

} // namespace beaker
