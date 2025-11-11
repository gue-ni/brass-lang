#include "brass.h"
#include "allocator.h"
#include "bytecode.h"
#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include <sstream>

int eval( const char * src, std::ostream & out, std::ostream & err )
{
  std::vector<Token> tokens = lex( src );

  NodeAllocator allocator;

  Result<Program> result = parse( tokens, allocator );

  if( !result.ok() )
  {
    err << result.error << std::endl;
    return 1;
  }

  GarbageCollector gc;

  CodeObject code;
  compile( result.node, gc, &code );

  VirtualMachine vm( out, err, gc );

  return vm.run( &code );
}

std::string repl_header()
{
  std::stringstream ss;
  ss << "Welcome to Brass! (Compiled " << __DATE__ << " " << __TIME__ << ")" << std::endl;
  return ss.str();
}

int repl()
{
  NodeAllocator allocator;

  GarbageCollector gc;

  VirtualMachine vm( std::cout, std::cerr, gc );

  CodeObject code_object;
  Compiler compiler( gc, &code_object );

  std::cout << repl_header() << std::endl;

  const std::string prompt = "> ";

  do
  {
    std::cout << prompt;

    std::string line;
    if( !std::getline( std::cin, line ) )
    {
      break;
    }

    if( line.empty() )
    {
      continue;
    }

    auto tokens = lex( line );
    if( tokens.empty() )
    {
      continue;
    }

    auto ast = parse( tokens, allocator );
    if( !ast.ok() )
    {
      std::cerr << ast.error << std::endl;
      continue;
    }

    ast.node->compile( compiler );

    vm.run( &code_object );
    code_object.instructions.clear();

  } while( true );
  return 0;
}

int brass( int argc, char * argv[] )
{
  return repl();
}
