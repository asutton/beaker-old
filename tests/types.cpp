// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/type.hpp"
#include "beaker/less.hpp"
#include "beaker/print.hpp"

#include <iostream>


using namespace lingo;
using namespace beaker;


void 
test_name()
{
  Type const* t1 = get_void_type();
  Type const* t2 = get_bool_type();
  Type const* t3 = get_int_type();
  Type const* t4 = get_function_type(Type_seq {t3, t3}, t2);
  Type const* t5 = get_reference_type(t3);

  std::cout << t1->node_name() << '\n';
  std::cout << t2->node_name() << '\n';
  std::cout << t3->node_name() << '\n';
  std::cout << t4->node_name() << '\n';
  std::cout << t5->node_name() << '\n';
}


bool
distinct(Type const* a, Type const* b)
{
  return less(a, b) || less(b, a);
}


bool
equivalent(Type const* a, Type const* b)
{
  return !less(a, b) && !less(b, a);
}

void 
test_less()
{
  Type const* t1 = get_void_type();
  Type const* t2 = get_bool_type();
  Type const* t3 = get_int_type();
  Type const* t4 = get_function_type(Type_seq {t3, t3}, t2);
  Type const* t5 = get_function_type(Type_seq {t2, t2}, t2);

  lingo_assert(equivalent(t1, t1));
  lingo_assert(equivalent(t2, t2));
  lingo_assert(equivalent(t3, t3));
  lingo_assert(equivalent(t4, t4));

  lingo_assert(distinct(t1, t2));
  lingo_assert(distinct(t1, t3));
  lingo_assert(distinct(t1, t4));
  lingo_assert(distinct(t4, t5));

  // Check identity.
  Type const* t6 = get_function_type(Type_seq {t3, t3}, t2);
  lingo_assert(t4 == t6);
}


void 
test_ref()
{
  Type const* v = get_void_type();
  Type const* b = get_bool_type();
  Type const* z = get_int_type();
  
  Type const* t1 = get_reference_type(b); // OK
  lingo_assert(is_valid_node(t1));
  
  Type const* t2 = get_reference_type(z); // OK
  lingo_assert(is_valid_node(t2));

  Type const* t3 = get_reference_type(v); // error: void is not an object type
  lingo_assert(is_error_node(t3));

  // TODO: Check composition of reference types.

  Type const* t4 = get_reference_type(b); // OK
  lingo_assert(distinct(t1, t2));
  lingo_assert(equivalent(t1, t4));
}


void 
test_print()
{
  Type const* t1 = get_void_type();
  Type const* t2 = get_bool_type();
  Type const* t3 = get_int_type();
  Type const* t4 = get_function_type(Type_seq {t3, t3}, t2);
  Type const* t5 = get_function_type(Type_seq {t2, t2}, t2);
  Type const* t6 = get_reference_type(t3);
  print(t1);
  print(t2);
  print(t3);
  print(t4);
  print(t5);
  print(t6);
}


int
main()
{
  test_name();
  test_less();
  test_ref();
  test_print();
}
