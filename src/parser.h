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
public:
  Parser( const std::vector<Token> & tokens, ArenaAllocator & arena );
  ParseResult<Program> run();

private:
  const std::vector<Token> m_tokens;
  std::vector<Token>::const_iterator m_pos;
  ArenaAllocator & m_arena;

  ParseResult<Program> parse_program();

  const Token & peek();
  const Token & previous();
  const Token & next();
  bool match( TokenType type );
  void expect( TokenType type, const std::string & lexeme = "" );
  bool is_finished();
};

ParseResult<Program> parse( const std::vector<Token> & tokens, ArenaAllocator & allocator );
