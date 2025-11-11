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
}

std::pair<uint16_t, bool> Compiler::find_var( const std::string & name )
{
  for( auto scope_it = scopes.rbegin(); scope_it != scopes.rend(); scope_it++ )
  {
    auto it = scope_it->find( name );
    if( it != scope_it->end() )
    {
      bool is_global = scope_it == std::prev( scopes.rend() );
      return std::make_pair( it->second, is_global );
    }
  }

  return std::make_pair( UNDEFINED, false );
}

uint16_t Compiler::define_var( const std::string & name )
{
  auto [index, is_global] = find_var( name );

  if( index != UNDEFINED )
  {
    return index;
  }
  else
  {
    if( scopes.size() == 1 )
    {
      auto & scope = scopes.back();
      uint16_t idx = code->emit_name( name );
      scope[name]  = idx;
      return idx;
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
}

