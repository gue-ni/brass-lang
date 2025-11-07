#pragma once

#include "bytecode.h"
#include "gc.h"
#include <ostream>
#include <stack>

struct Frame
{
  Frame( CodeObject * co )
      : code_object( co )
  {
    ip = code_object->instructions.cbegin();
  }
  CodeObject * code_object;
  Instructions::const_iterator ip;
  std::stack<Object *> stack;
};

class VirtualMachine
{
public:
  VirtualMachine( std::ostream & out, std::ostream & err, GarbageCollector& gc );
  int run( CodeObject * );

private:
  bool m_exit = false;
  std::ostream & m_out;
  std::ostream & m_err;
  GarbageCollector& m_gc;
  std::stack<Frame> m_frames;

  void push(Object*);
  Object* pop();
  Frame & current_frame();
  std::pair<Instruction, uint16_t> next_instr();
};
