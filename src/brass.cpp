#include "brass.h"
#include "allocator.h"
#include "bytecode.h"
#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include <fstream>
#include <sstream>

int eval( const char * src, std::ostream & out, std::ostream & err )
{
  std::vector<Token> tokens = lex( src );

  GarbageCollector gc;
  NodeAllocator allocator;

  Result<Program> result = parse( tokens, allocator, gc );
  if( !result.ok() )
  {
    err << "PARSER ERROR: " << result.error << std::endl;
    return 1;
  }

  TypeContext ctx;
  result.node->check_types( ctx );
  if( !ctx.ok() )
  {
    err << "TYPE ERROR: " << ctx.error << std::endl;
  }

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
  GarbageCollector gc;
  NodeAllocator allocator;

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

    auto ast = parse( tokens, allocator, gc );
    if( !ast.ok() )
    {
      std::cerr << ast.error << std::endl;
      continue;
    }

    TypeContext ctx;
    ast.node->check_types( ctx );
    if( !ctx.ok() )
    {
      std::cerr << "TYPE ERROR: " << ctx.error << std::endl;
    }

    ast.node->compile( compiler );

    vm.run( &code_object );
    code_object.instructions.clear();

  } while( true );
  return 0;
}

int brass( int argc, char * argv[] )
{
  if( 1 < argc )
  {
    std::string filename = argv[1];

    std::fstream file( filename );
    if( !file.is_open() )
    {
      std::cerr << "Could not open '" << filename << "'" << std::endl;
      return 1;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string src = ss.str();

    if( src.empty() )
    {
      std::cerr << "File '" << filename << "' is empty" << std::endl;
      return 1;
    }

    return eval( src.c_str(), std::cout, std::cerr );
  }
  else
  {
    return repl();
  }
}
