#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "object.h"

enum Instruction : uint8_t
{
  OP_LOAD_CONST,
  OP_BINARY_ADD,
  OP_DEBUG_PRINT,
  OP_LOAD_VAR,
  OP_STORE_VAR,
  OP_CALL_FUNCTION,
  OP_CALL_METHOD,
};

struct CodeObject
{
  std::vector<Value> literals;
  std::vector<uint8_t> instructions;
  std::vector<std::string> variables;
  void emit_instr(Instruction);
  void emit_literal(Value);
};
