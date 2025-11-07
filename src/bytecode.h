#pragma once

#include <vector>

class Bytecode
{
public:
  Bytecode();

private:
  std::vector<std::vector<uint8_t>> mCode;
};
