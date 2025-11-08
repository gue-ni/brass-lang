#include "compiler.h"
#include "ast.h"

CodeObject compile( AstNode * ast, GarbageCollector& gc )
{
  CodeObject code;
  Compiler compiler(gc, &code);
  ast->compile( compiler );
  return code;
}
