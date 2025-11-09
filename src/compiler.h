#pragma once
#include "bytecode.h"
#include "gc.h"

#include <list>
#include <map>

struct AstNode;

#define UNDEFINED 0xffff

struct Compiler
{
  GarbageCollector & gc;
  CodeObject * code;

  uint16_t scope_offset;
  uint16_t num_locals;
  std::list<std::map<std::string, uint16_t>> scopes;

  Compiler( GarbageCollector & gc, CodeObject * code )
      : gc( gc )
      , code( code )
      , scope_offset(0)
    , num_locals(0)
  {
  }

  void push_scope();
  void pop_scope();
uint16_t find_in_scopes(const std::string& name) ;
  uint16_t define_local(const std::string& name);
};

CodeObject compile( AstNode *, GarbageCollector & gc );