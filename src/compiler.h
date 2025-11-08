#pragma once
#include "bytecode.h"
#include "gc.h"

struct AstNode;

struct Compiler
{
  GarbageCollector & gc;
  CodeObject code;
  Compiler( GarbageCollector & gc )
      : gc( gc )
  {
  }
};

CodeObject compile( AstNode * , GarbageCollector& gc);