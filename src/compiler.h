#pragma once
#include "bytecode.h"
#include "gc.h"

struct AstNode;

struct Compiler
{
  GarbageCollector & gc;
  CodeObject *code;
  Compiler( GarbageCollector & gc, CodeObject* code )
      : gc( gc ), code(code)
  {
  }
};

CodeObject compile( AstNode * , GarbageCollector& gc);