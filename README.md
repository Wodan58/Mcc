# Mcc
Learning compiler

Build|Linux|Windows
---|---|---
status|[![GitHub CI build status](https://github.com/Wodan58/Mcc/actions/workflows/cmake.yml/badge.svg)](https://github.com/Wodan58/Mcc/actions/workflows/cmake.yml)|[![AppVeyor CI build status](https://ci.appveyor.com/api/projects/status/jcjka1b5vovhwmyk?svg=true)](https://ci.appveyor.com/project/Wodan58/Mcc)

This project implements a C compiler, following the road map of
[chibicc](https://github.com/rui314/chibicc).

The best way to learn is to follow every step, doing everything differently.

Chibicc uses malloc, no free. I'll use GC_malloc instead. I don't mind using
the [BDW garbage collector](https://github.com/ivmai/bdwgc) and the aim is not
to build the smallest possible binary.

Chibicc uses a recursive descent parser. I'll use flex and bison instead. This
increases the size of the binary and decreases the amount of lines of code.

Chibicc compiles directly to x86_64 assembly, using AT&T syntax. I'll compile
to byte code that can either be interpreted or converted to assembly later on.

Chibicc uses the normal call/ret of x86 machine code. ~I'll be building a
stackless version instead. TBH there is a stack, but it will be allocated on
the heap.~

Chibicc uses make as the build system. I'll use Cmake instead.

In short, I'll follow the 345+ steps that are needed to build a modern C
compiler, but I'll do it my way.

The aim is to learn. The final product is of secondary importance.

INSTALL
-------

    mkdir build
    cd build
    cmake ..
    cmake --build .
