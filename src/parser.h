#pragma once

#include "allocator.h"
#include "ast.h"
#include "lexer.h"

#include <string>
#include <vector>

template <typename NodeType>
struct Result
{
  std::string error;
  NodeType * node;
  bool ok() const
  {
    return error.empty() && node != nullptr;
  }
};

template <typename T>
Result<T> make_result( T * node )
{
  Result<T> r;
  r.node = node;
  return r;
}

template <typename T>
Result<T> make_error( const std::string & error )
{
  Result<T> r;
  r.error = error;
  return r;
}

class Parser
{
public:
  Parser( const std::vector<Token> & tokens, ArenaAllocator & arena );
  Result<Program> run();

private:
  const std::vector<Token> m_tokens;
  std::vector<Token>::const_iterator m_pos;
  ArenaAllocator & m_arena;

  Result<Program> parse_program();
  Result<Stmt> parse_stmt();
  Result<Block> parse_block();
  Result<Expr> parse_expr();
  Result<Expr> parse_primary();
  Result<Expr> parse_term();
  Result<Expr> parse_factor();

  const Token & peek();
  const Token & previous();
  const Token & next();
  bool match( TokenType type );
  void expect( TokenType type, const std::string & lexeme = "" );
  bool is_finished();
};

Result<Program> parse( const std::vector<Token> & tokens, ArenaAllocator & allocator );
