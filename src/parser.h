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

  Result<Program> parse_program();
  Result<Stmt> parse_stmt();
  Result<FnDecl> parse_fn_decl();
  Result<ClassDecl> parse_class_decl();
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

Result<Program> parse( const std::vector<Token> & tokens, NodeAllocator & allocator );
