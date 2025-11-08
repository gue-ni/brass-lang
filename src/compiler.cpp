#include "compiler.h"
#include "ast.h"

CodeObject compile( AstNode * ast, GarbageCollector& gc )
{
  Compiler compiler(gc);
  ast->compile( compiler );
  return compiler.code;
}
