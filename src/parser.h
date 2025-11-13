#pragma once

#include "allocator.h"
#include "ast.h"
#include "lexer.h"

#include <string>
#include <vector>

class Parser
{
public:
  Parser( const std::vector<Token> & tokens, NodeAllocator & arena );
  Result<Program> run();

private:
  const std::vector<Token> m_tokens;
  std::vector<Token>::const_iterator m_pos;
  NodeAllocator & m_arena;

  Result<Stmt> parse_statement();
  Result<Stmt> parse_fn_decl();
  Result<Stmt> parse_var_decl();
  Result<Stmt> parse_declaration();
  Result<Stmt> parse_class_decl();
  Result<Stmt> parse_block();
  Result<Stmt> parse_expr_stmt();

  Result<Expr> parse_expression();
  Result<Expr> parse_expr();
  Result<Expr> parse_primary();
  Result<Expr> parse_unary();
  Result<Expr> parse_term();
  Result<Expr> parse_factor();
  Result<Expr> parse_assignment();
  Result<Expr> parse_call();

  const Token & peek();
  const Token & previous();
  const Token & next();
  bool match( TokenType type );
  bool is_finished();
};

Result<Program> parse( const std::vector<Token> & tokens, NodeAllocator & allocator );
