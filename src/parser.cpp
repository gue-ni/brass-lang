#include "parser.h"

ParseResult<Program> parse( const std::vector<Token> & tokens, ArenaAllocator & allocator )
{
  Program * ast = allocator.alloc<Program>();
  return make_result( ast );
}
