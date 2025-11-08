#include "brass.h"
#include "allocator.h"
#include "bytecode.h"
#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include "vm.h"

int eval( const char * src, std::ostream & out, std::ostream & err )
{
  std::vector<Token> tokens = lex( src );

  ArenaAllocator allocator( 4096 );
  GarbageCollector gc;

  Result<Program> result = parse( tokens, allocator );

  if( !result.is_ok() )
  {
    err << result.error << std::endl;
    return 1;
  }

  CodeObject code = compile( result.node );

  VirtualMachine vm(out, err, gc);

  return vm.run( &code );
}
