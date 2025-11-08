#include "parser.h"
#include <iostream>

Parser::Parser( const std::vector<Token> & tokens, ArenaAllocator & arena )
    : m_tokens( tokens )
    , m_pos( m_tokens.begin() )
    , m_arena( arena )
{
}

ParseResult<Program> Parser::run()
{
  return parse_program();
}

const Token & Parser::previous()
{
  return *( m_pos - 1 );
}

ParseResult<Program> Parser::parse_program()
{
  Program * ast = m_arena.alloc<Program>();
  return make_result( ast );
}

const Token & Parser::peek()
{
  return *( m_pos );
}

const Token & Parser::next()
{
  return *( m_pos++ );
}

bool Parser::match( TokenType type )
{
  if( !is_finished() && ( peek().type == type ) )
  {
    next();
    return true;
  }
  else
  {
    return false;
  }
}

void Parser::expect( TokenType type, const std::string & lexeme )
{
  const Token & token = peek();
  if( token.type != type /* || ( !peek().lexeme.empty() && peek().lexeme != lexeme ) */ )
  {
    std::cerr << "Unexpected token '" << peek().lexeme << "'" << std::endl;
    std::abort();
  }

  ( void ) next();
}

bool Parser::is_finished()
{
  return m_pos == m_tokens.end();
}

ParseResult<Program> parse( const std::vector<Token> & tokens, ArenaAllocator & allocator )
{
  Parser parser( tokens, allocator );
  return parser.run();
}
