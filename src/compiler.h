#pragma once
#include "bytecode.h"

struct AstNode;

class Compiler {
public:
  CodeObject code;
};

CodeObject compile( AstNode * );