#pragma once

#include "utils.h"
#include <string>
#include <vector>

enum TokenType : uint
{
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  SEMICOLON,
  TILDE,
  MINUS,
  MINUS_MINUS,
  PLUS,
  PLUS_PLUS,
  STAR,
  SLASH,
  EQUAL,
  EQUAL_EQUAL,

  // type specifiers
  KW_VOID,
  KW_INT,
  KW_CHAR,
  KW_SHORT,
  KW_LONG,
  KW_FLOAT,
  KW_DOUBLE,
  KW_SIGNED,
  KW_UNSIGNED,

  // type qualifiers
  KW_CONST,
  KW_VOLATILE,
  KW_RESTRICT,
  KW_STATIC,

  // control flow
  KW_IF,
  KW_ELSE,
  KW_FOR,
  KW_WHILE,
  KW_RETURN,

  // literals
  NUMBER,
  STRING,

  IDENTIFIER,
};

struct Token
{
  const TokenType type;
  const std::string lexeme;

  Token( TokenType tt, const std::string & lx )
      : type( tt )
      , lexeme( lx )
  {
  }

  Token( TokenType tt, char c )
      : type( tt )
      , lexeme( std::string( 1, c ) )
  {
  }

  std::string to_string() const;
};

class Lexer
{
public:
  Lexer( const std::string & src );
  std::vector<Token> tokens() const;

private:
  const std::string m_source;
  std::string::const_iterator m_pos;
  std::vector<Token> m_tokens;

  void run();
  void push_token( const Token & token );
  bool is_finished() const;
  void skip_whitespace();
  bool is_identifier( char );
  bool is_numeric( char );
  void handle_identifier();
  void handle_number();
  char next();
  char peek() const;
  char peek_next() const;
  bool match_next( char );
};

std::vector<Token> lex( const std::string & src );
