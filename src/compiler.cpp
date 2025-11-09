#include "compiler.h"
#include "ast.h"

CodeObject compile( AstNode * ast, GarbageCollector & gc )
{
  CodeObject code;
  Compiler compiler( gc, &code );
  ast->compile( compiler );
  return code;
}

void Compiler::push_scope()
{
  scopes.push_back( {} );
}

void Compiler::pop_scope()
{
  scope_offset -= scopes.back().size();
  scopes.pop_back();

  #if 0
  if (scopes.empty()) {
    code->num_locals = 0; // reset local count
  }
  #endif
}

uint16_t Compiler::find_in_scopes( const std::string & name )
{
  for( auto scope_it = scopes.rbegin(); scope_it != scopes.rend(); scope_it++ )
  {
    auto it = scope_it->find( name );
    if( it != scope_it->end() )
    {
      return it->second;
    }
  }

  return UNDEFINED;
}

uint16_t Compiler::define_local( const std::string & name )
{
  uint16_t index = find_in_scopes( name );

  if( index != UNDEFINED )
  {
    return index;
  }
  else
  {
    code->num_locals++;
    auto & scope    = scopes.back();
    uint16_t offset = scope_offset++;
    scope[name]     = offset;
    return offset;
  }
}
