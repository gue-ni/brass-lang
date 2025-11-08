#include "compiler.h"
#include "ast.h"

CodeObject compile( AstNode * ast )
{
  Compiler compiler;
  ast->compile( compiler );
  return compiler.code;
}
