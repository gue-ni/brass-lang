#include "parser.h"
#include <iostream>

Parser::Parser( const std::vector<Token> & tokens, ArenaAllocator & arena )
    : m_tokens( tokens )
    , m_pos( m_tokens.begin() )
    , m_arena( arena )
{
}

Result<Program> Parser::run()
{
  return parse_program();
}

const Token & Parser::previous()
{
  return *( m_pos - 1 );
}

Result<Program> Parser::parse_program()
{
  Program * prog = m_arena.alloc<Program>();

  do
  {
    auto result = parse_stmt();
    if( !result.ok() )
    {
      return make_error<Program>( result.error );
    }

    prog->stmts.push_back( result.node );
  } while( !is_finished() );

  return make_result( prog );
}

Result<Stmt> Parser::parse_stmt()
{
  bool expect_semicolon = true;
  Stmt * stmt           = nullptr;
  if( match( KW_PRINT ) )
  {
    if( !match( LPAREN ) )
    {
      return make_error<Stmt>( "Expected '('" );
    }

    Result<Expr> expr = parse_expr();
    if( !expr.ok() )
    {
      return make_error<Stmt>( expr.error );
    }

    if( !match( RPAREN ) )
    {
      return make_error<Stmt>( "Expected ')'" );
    }

    stmt = m_arena.alloc<DebugPrint>( expr.node );
  }
  else if( match( KW_FN ) )
  {
    if( !match( IDENTIFIER ) )
    {
      return make_error<Stmt>( "Expected identifier after 'fn'" );
    }

    std::string fn_name = previous().lexeme;

    if( !match( LPAREN ) )
    {
      return make_error<Stmt>( "Expected '('" );
    }
    if( !match( RPAREN ) )
    {
      return make_error<Stmt>( "Expected ')'" );
    }

    if( !match( LBRACE ) )
    {
      return make_error<Stmt>( "Expected '{'" );
    }

    auto body = parse_block();
    if( !body.ok() )
    {
      return make_error<Stmt>( body.error );
    }

    if( !match( RBRACE ) )
    {
      return make_error<Stmt>( "Expected '}'" );
    }

    std::vector<std::string> params;
    stmt             = m_arena.alloc<FnDecl>( fn_name, params, body.node );
    expect_semicolon = false;
  }
  else if( match( KW_RETURN ) )
  {
    // TODO
    Result<Expr> expr = parse_expr();
    if( !expr.ok() )
    {
      return make_error<Stmt>( expr.error );
    }

    stmt = m_arena.alloc<Return>( expr.node );
  }
  else if( match( KW_VAR ) )
  {
    if( !match( IDENTIFIER ) )
    {
      return make_error<Stmt>( "Expected identifier" );
    }

    std::string name = previous().lexeme;

    if( !match( EQUAL ) )
    {
      return make_error<Stmt>( "Expected '='" );
    }

    auto expr = parse_expr();
    if( !expr.ok() )
    {
      return make_error<Stmt>( expr.error );
    }

    stmt = m_arena.alloc<VariableDecl>( name, expr.node );
  }
  else if( match( IDENTIFIER ) )
  {
    std::string name = previous().lexeme;

    if( !match( EQUAL ) )
    {
      return make_error<Stmt>( "Expected '='" );
    }

    auto expr = parse_expr();
    if( !expr.ok() )
    {
      return make_error<Stmt>( expr.error );
    }

    stmt = m_arena.alloc<Assignment>( name, expr.node );
  }

  if( !match( SEMICOLON ) )
  {
    if( expect_semicolon )
    {
      return make_error<Stmt>( "Expected ';'" );
    }
  }

  return make_result( stmt );
}

Result<Block> Parser::parse_block()
{
  Block * block = m_arena.alloc<Block>();
  do
  {
    if( peek().type == RBRACE )
    {
      break;
    }

    auto result = parse_stmt();
    if( !result.ok() )
    {
      return make_error<Block>( result.error );
    }

    block->stmts.push_back( result.node );
  } while( !is_finished() );

  return make_result( block );
}

Result<Expr> Parser::parse_expr()
{
  return parse_term();
}

Result<Expr> Parser::parse_primary()
{
  if( match( NUMBER ) )
  {
    int value         = std::stoi( previous().lexeme );
    Literal * literal = m_arena.alloc<Literal>( Object::Integer( value ) );
    return make_result<Expr>( literal );
  }
  else if( match( IDENTIFIER ) )
  {
    Variable * var = m_arena.alloc<Variable>( previous().lexeme );

    if( !match( LPAREN ) )
    {
      return make_result<Expr>( var );
    }

    std::vector<Expr *> args;

    if( !match( RPAREN ) )
    {
      return make_error<Expr>( "Expected ')'" );
    }

    FnCall * fn_call = m_arena.alloc<FnCall>( var, args );
    return make_result<Expr>( fn_call );
  }
  else
  {
    return make_error<Expr>( "Not Implemented" );
  }
}

Result<Expr> Parser::parse_term()
{
  Expr * expr = nullptr;
  auto left   = parse_factor();
  if( !left.ok() )
  {
    return make_error<Expr>( left.error );
  }

  if( match( PLUS ) )
  {
    auto right = parse_factor();
    if( !right.ok() )
    {
      return make_error<Expr>( right.error );
    }
    expr = m_arena.alloc<Binary>( left.node, right.node );
  }
  else
  {
    expr = left.node;
  }

  return make_result( expr );
}

Result<Expr> Parser::parse_factor()
{
  return parse_primary();
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

Result<Program> parse( const std::vector<Token> & tokens, ArenaAllocator & allocator )
{
  Parser parser( tokens, allocator );
  return parser.run();
}
