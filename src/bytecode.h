#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class Object;

enum Instruction : uint8_t
{
  OP_NOP,
  OP_LOAD_CONST,
  OP_LOAD_GLOBAL,
  OP_STORE_GLOBAL,
  OP_LOAD_LOCAL,
  OP_STORE_LOCAL,
  OP_MAKE_FUNCTION,
  OP_CALL_FUNCTION,
  OP_RETURN,
  OP_ADD,
  OP_SUB,
  OP_DEBUG_PRINT,
};

struct CodeObject
{
  uint16_t num_locals = 0;
  std::vector<Object> literals;
  std::vector<uint8_t> instructions;
  std::vector<std::string> names;
  void emit_instr( Instruction );
  void emit_instr( Instruction, uint16_t );
  void emit_literal( Object );
  uint16_t emit_name( const std::string & name );
};
