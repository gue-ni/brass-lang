#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class Object;

enum OpCode : uint8_t
{
  OP_NOP,
  OP_LOAD_CONST,
  OP_LOAD_GLOBAL,
  OP_STORE_GLOBAL,
  OP_LOAD_LOCAL,
  OP_STORE_LOCAL,
  OP_CALL,
  OP_SET_PROPERTY,
  OP_GET_PROPERTY,
  OP_RETURN,
  OP_ADD,
  OP_SUB,
  OP_PRINT,
  OP_PRINTLN,
  OP_JMP,
  OP_JMP_IF_FALSE,
  OP_LOOP,
};

struct CodeObject
{
  CodeObject* parent = nullptr;
  uint16_t num_locals = 0;
  std::vector<Object> literals;
  std::vector<uint8_t> instructions;
  std::vector<std::string> names; // local names
  void emit_instr( OpCode );
  void emit_instr( OpCode, uint16_t );
  void emit_literal( Object );
  uint16_t emit_name( const std::string & name );
  CodeObject* get_root();
};
