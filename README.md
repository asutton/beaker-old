
The Beaker Programming Language is a small C-like programming
language that supports integer and boolean types, mutable
variables, and functions on objects.

The specification can be found in the docs directory. It is
built using the command `make spec`.

This repository contains an implementation of a compiler and
interpreter for the Beaker programming language. Both the
compiler and the interpreter read a Beaker program from 
a file. The interpreter will execute the program, and the
compiler will translate the output to LLVM.

