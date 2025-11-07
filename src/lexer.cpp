#include "lexer.h"
#include <iostream>
#include <map>

std::string Token::to_string() const
{
  return "TOKEN(" + std::to_string( type ) + " '" + lexeme + "')";
}

Lexer::Lexer( const std::string & src )
    : m_source( src )
    , m_pos( m_source.begin() )
{
  run();
}

std::vector<Token> Lexer::tokens() const
{
  return m_tokens;
}

bool Lexer::is_finished() const
{
  return m_pos == m_source.end();
}

// TODO: handle comments
// TODO: keep track of lines + columns for error messages
void Lexer::skip_whitespace()
{
  while( is_finished() == false )
  {
    if( isspace( peek() ) )
    {
      ( void ) next();
    }
    else
    {
      break;
    }
  }
}

char Lexer::next()
{
  if( is_finished() )
    return '\0';
  return *( m_pos++ );
}

char Lexer::peek() const
{
  if( is_finished() )
    return '\0';
  return *( m_pos );
}

char Lexer::peek_next() const
{
  if( ( m_pos + 1 ) != m_source.end() )
  {
    return *( m_pos + 1 );
  }
  else
  {
    return '\0';
  }
}

bool Lexer::match_next( char c )
{
  if( peek() == c )
  {
    ( void ) next();
    return true;
  }
  else
  {
    return false;
  }
}

void Lexer::push_token( const Token & token )
{
  m_tokens.push_back( token );
}

bool Lexer::is_identifier( char c )
{
  return isalnum( c ) || c == '_';
}

bool Lexer::is_numeric( char c )
{
  return isdigit( c );
}

void Lexer::handle_number()
{
  auto start = m_pos - 1;

  while( std::isdigit( peek() ) )
  {
    next();
  }

  if( peek() == '.' && std::isdigit( peek_next() ) )
  {
    next();
  }

  while( std::isdigit( peek() ) )
  {
    next();
  }

  auto end = m_pos;
  std::string number( start, end );

  push_token( Token( NUMBER, number ) );
}

void Lexer::handle_identifier()
{
  static const std::map<std::string, TokenType> keywords = {
      // clang-format off
      { "void", KW_VOID },
      { "int", KW_INT },
      { "char", KW_CHAR },
      { "short", KW_SHORT },
      { "long", KW_LONG },
      { "float", KW_FLOAT },
      { "double", KW_DOUBLE },
      { "signed", KW_SIGNED },
      { "unsigned", KW_UNSIGNED },
      { "const", KW_CONST },
      { "volatitle", KW_VOLATILE },
      { "restrict", KW_RESTRICT },
      { "static", KW_STATIC },
      { "if", KW_IF },
      { "else", KW_ELSE },
      { "for", KW_FOR },
      { "while", KW_WHILE },
      { "return", KW_RETURN },
      // clang-format on
  };

  auto start = m_pos - 1;

  while( is_identifier( peek() ) )
  {
    next();
  }

  auto end = m_pos;

  std::string identifier( start, end );

  auto it = keywords.find( identifier );
  if( it != keywords.end() )
  {
    push_token( Token( it->second, it->first ) );
  }
  else
  {
    push_token( Token( IDENTIFIER, identifier ) );
  }
};

void Lexer::run()
{
  bool error = false;
  while( !is_finished() && !error )
  {
    skip_whitespace();
    char c = next();
    switch( c )
    {
      case '\0' :
        break;
      case ';' :
        push_token( Token( SEMICOLON, c ) );
        break;
      case '(' :
        push_token( Token( LPAREN, c ) );
        break;
      case ')' :
        push_token( Token( RPAREN, c ) );
        break;
      case '{' :
        push_token( Token( LBRACE, c ) );
        break;
      case '}' :
        push_token( Token( RBRACE, c ) );
        break;
      case '~' :
        push_token( Token( TILDE, c ) );
        break;
      case '*' :
        push_token( Token( STAR, c ) );
        break;
      case '/' :
        push_token( Token( SLASH, c ) );
        break;
      case '+' :
        push_token( match_next( '+' ) ? Token( PLUS_PLUS, "++" ) : Token( PLUS, c ) );
        break;
      case '-' :
        push_token( match_next( '-' ) ? Token( MINUS_MINUS, "--" ) : Token( MINUS, c ) );
        break;
      case '=' :
        push_token( match_next( '=' ) ? Token( EQUAL_EQUAL, "==" ) : Token( EQUAL, c ) );
        break;
      default :
        {
          if( is_numeric( c ) )
          {
            handle_number();
          }
          else if( is_identifier( c ) )
          {
            handle_identifier();
          }
          else
          {
            std::cerr << "Unhandled character: '" << c << "'" << std::endl;
            error = true;
          }
        }
    }
  }
}

std::vector<Token> lex( const std::string & src )
{
  Lexer lexer( src );
  return lexer.tokens();
}
