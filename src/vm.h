#pragma once

#include "bytecode.h"
#include "gc.h"
#include "object.h"

#include <map>
#include <ostream>
#include <stack>


Object f_typeof( int argc, Object args[] );

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
  std::string m_runtime_error_message;

  void push( Object );
  Object pop();
  Frame & current_frame();
  CodeObject * current_code_object();
  CodeObject * global_code_object();
  std::pair<OpCode, uint16_t> next_instr();
  void call_fn( FunctionObject * );
  void call_ctor( ClassObject * );
};
