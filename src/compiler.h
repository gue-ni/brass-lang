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
  std::list<std::map<std::string, uint16_t>> scopes;

  Compiler( GarbageCollector & gc, CodeObject * code )
      : gc( gc )
      , code( code )
      , scope_offset( 0 )
  {
    scopes.push_back( {} ); // global scope
  }

  void push_scope();
  void pop_scope();
  std::pair<uint16_t, bool> find_var( const std::string & name );
  uint16_t define_var( const std::string & name );
};

CodeObject compile( AstNode *, GarbageCollector & gc );
