#pragma once

#include <cstddef>
#include <string>
#include <vector>

class Object;

enum Instruction : uint8_t
{
  OP_LOAD_CONST,
  OP_LOAD_VAR,
  OP_STORE_VAR,
  OP_LOAD_FAST,
  OP_MAKE_FUNCTION,
  OP_CALL_FUNCTION,
  OP_RETURN,
  OP_ADD,
  OP_DEBUG_PRINT,
};

struct CodeObject
{
  std::vector<Object> literals;
  std::vector<uint8_t> instructions;
  std::vector<std::string> names;
  void emit_instr( Instruction );
  void emit_instr( Instruction, uint16_t );
  void emit_literal( Object );
  uint16_t emit_name( const std::string & name );
};
