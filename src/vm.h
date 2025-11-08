#pragma once

#include "bytecode.h"
#include "gc.h"
#include <ostream>
#include <stack>

struct Frame
{
  CodeObject * code_object;
  std::vector<uint8_t>::const_iterator ip;
  std::stack<Value> stack;

  Frame( CodeObject * co )
      : code_object( co )
      , ip( code_object->instructions.begin() )
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

  void push( Value );
  Value pop();
  Frame & current_frame();
  std::pair<Instruction, uint16_t> next_instr();
};
