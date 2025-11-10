#pragma once

#include "bytecode.h"
#include "gc.h"
#include "object.h"

#include <map>
#include <ostream>
#include <stack>

struct Frame
{
  CodeObject * code_object;
  std::vector<uint8_t>::const_iterator ip;
  size_t bp;

  Frame( CodeObject * co, size_t bp = 0 )
      : code_object( co )
      , ip( code_object->instructions.begin() )
      , bp( bp )
  {
  }
};

class VirtualMachine
{
public:
  VirtualMachine( std::ostream & out, std::ostream & err, GarbageCollector & gc );
  int run( CodeObject * );

private:
  bool m_exit = false;
  std::ostream & m_out;
  std::ostream & m_err;
  GarbageCollector & m_gc;
  std::stack<Frame> m_frames;
  std::vector<Object> m_stack;
  std::map<std::string, Object> m_globals;

  void push( Object );
  Object pop();
  Frame & current_frame();
  std::pair<Instruction, uint16_t> next_instr();
  void call_fn( FunctionObject * fn );
};
