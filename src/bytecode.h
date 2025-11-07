#pragma once

#include <cstddef>
#include <vector>
#include <string>


class Object;

enum Instruction : uint8_t
{
  LOAD_CONST,
  BINARY_ADD,
  LOAD_VAR,
  STORE_VAR,
  DEBUG_PRINT,
};

using Instructions = std::vector<uint8_t>;
using Literals     = std::vector<Object *>;

struct CodeObject
{
  Literals literals;
  Instructions instructions;
  std::vector<std::string> variables;
};
