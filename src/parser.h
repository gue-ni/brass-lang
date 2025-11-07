#pragma once

#include "allocator.h"
#include "ast.h"
#include "lexer.h"

#include <string>
#include <vector>

template <typename NodeType>
struct ParseResult
{
  std::string error;
  NodeType * node;
  bool is_ok() const
  {
    return error.empty() && node != nullptr;
  }
};

template <typename T>
ParseResult<T> make_result( T * node )
{
  ParseResult<T> r;
  r.node = node;
  return r;
}

template <typename T>
ParseResult<T> make_error( const std::string & error )
{
  ParseResult<T> r;
  r.error = "Parser Error: " + error;
  return r;
}

class Parser
{
};


ParseResult<Program> parse( const std::vector<Token> & tokens, ArenaAllocator & allocator );
