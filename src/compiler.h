#pragma once
#include "bytecode.h"

struct AstNode;

struct Compiler
{
  CodeObject code;
};

CodeObject compile( AstNode * );