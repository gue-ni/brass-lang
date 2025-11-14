#include "parser.h"
#include "ast.h"
#include <iostream>

Parser::Parser( const std::vector<Token> & tokens, NodeAllocator & arena )
    : m_tokens( tokens )
    , m_pos( m_tokens.begin() )
    , m_arena( arena )
{
}

Result<Program> Parser::run()
{
  Program * prog = m_arena.alloc<Program>();

  do
  {
    auto result = parse_declaration();
    if( !result.ok() )
    {
      return make_error<Program>( result.error );
    }

    prog->stmts.push_back( result.node );
  } while( !is_finished() );

  return make_result( prog );
}

Result<Stmt> Parser::parse_statement()
{
  if( match( KW_PRINT ) || match( KW_PRINTLN ) )
  {
    bool newline = previous().type == KW_PRINTLN ? true : false;

    Result<Expr> expr = parse_expression();
    if( !expr.ok() )
      return make_error<Stmt>( expr.error );

    if( !match( SEMICOLON ) )
      return make_error<Stmt>( "Expected ';' after 'print' statement" );

    return make_result<Stmt>( m_arena.alloc<Print>( expr.node, newline ) );
  }
  else if( match( KW_RETURN ) )
  {
    Result<Expr> expr = parse_expression();
    if( !expr.ok() )
      return make_error<Stmt>( expr.error );

    if( !match( SEMICOLON ) )
      return make_error<Stmt>( "Expected ';' after 'return' statement" );

    return make_result<Stmt>( m_arena.alloc<Return>( expr.node ) );
  }
  else if( match( KW_IF ) )
  {
    return parse_if();
  }
  else if( match( KW_WHILE ) )
  {
    return parse_while();
  }
  else if( match( LBRACE ) )
  {
    return parse_block();
  }
  else
  {
    return parse_expr_stmt();
  }
}

Result<Stmt> Parser::parse_fn_decl()
{
  if( !match( IDENTIFIER ) )
    return make_error<Stmt>( "Expected identifier after 'fn'" );

  std::string fn_name = previous().lexeme;

  if( !match( LPAREN ) )
    return make_error<Stmt>( "Expected '(' after function name" );

  std::vector<std::string> args;

  do
  {
    if( peek().type == RPAREN )
      break;

    if( !match( IDENTIFIER ) )
      return make_error<Stmt>( "Expected identifier" );

    args.push_back( previous().lexeme );

    ( void ) match( COMMA );
  } while( !is_finished() );

  if( !match( RPAREN ) )
    return make_error<Stmt>( "Expected ')'" );

  if( !match( LBRACE ) )
    return make_error<Stmt>( "Expected '{'" );

  auto body = parse_block();

  if( !body.ok() )
    return make_error<Stmt>( body.error );

  return make_result<Stmt>( m_arena.alloc<FnDecl>( fn_name, args, body.node ) );
}

Result<Stmt> Parser::parse_var_decl()
{
  if( !match( IDENTIFIER ) )
    return make_error<Stmt>( "Expected identifier in variable declaration" );

  std::string name = previous().lexeme;

  if( !match( EQUAL ) )
    return make_error<Stmt>( "Expected '=' in variable declaration" );

  auto expr = parse_expression();
  if( !expr.ok() )
    return make_error<Stmt>( expr.error );

  if( !match( SEMICOLON ) )
    return make_error<Stmt>( "Expected ';' after variable declaration" );

  return make_result<Stmt>( m_arena.alloc<VariableDecl>( name, expr.node ) );
}

Result<Expr> Parser::parse_assignment()
{
  auto expr = parse_term();
  if( !expr.ok() )
    return make_error<Expr>( expr.error );

  if( match( EQUAL ) )
  {
    auto value = parse_assignment();

    if( dynamic_cast<Variable *>( expr.node ) )
    {
      Variable * var = ( Variable * ) expr.node;
      return make_result<Expr>( m_arena.alloc<Assignment>( var->name, value.node ) );
    }
    else if( dynamic_cast<Get *>( expr.node ) != nullptr )
    {
      Get * get = ( Get * ) expr.node;
      return make_result<Expr>( m_arena.alloc<Set>( get->object, get->property, value.node ) );
    }

    return make_error<Expr>( "assigment not impelmented" );
  }
  else
  {
    return expr;
  }
}

Result<Expr> Parser::parse_call()
{
  auto expr = parse_primary();
  if( !expr.ok() )
    return make_error<Expr>( expr.error );

  if( match( LPAREN ) )
  {
    std::vector<Expr *> args;

    do
    {
      if( peek().type == RPAREN )
      {
        break;
      }

      auto arg = parse_expression();
      if( !arg.ok() )
      {
      }

      args.push_back( arg.node );

      ( void ) match( COMMA );
    } while( !is_finished() );

    if( !match( RPAREN ) )
    {
      return make_error<Expr>( "Expected ')'" );
    }

    Call * fn_call = m_arena.alloc<Call>( expr.node, args );
    return make_result<Expr>( fn_call );
  }
  else if( match( DOT ) )
  {
    if( !match( IDENTIFIER ) )
    {
      return make_error<Expr>( "expected identifier" );
    }

    std::string name = previous().lexeme;
    Get * get        = m_arena.alloc<Get>( expr.node, name );
    return make_result<Expr>( get );
  }
  else
  {
    return expr;
  }
}

Result<Stmt> Parser::parse_declaration()
{
  if( match( KW_CLASS ) )
  {
    return parse_class_decl();
  }
  else if( match( KW_FN ) )
  {
    return parse_fn_decl();
  }
  else if( match( KW_VAR ) )
  {
    return parse_var_decl();
  }
  else
  {
    return parse_statement();
  }
}

Result<Stmt> Parser::parse_class_decl()
{
  if( !match( IDENTIFIER ) )
    return make_error<Stmt>( "Expected class name" );

  std::string name = previous().lexeme;

  if( !match( LBRACE ) )
    return make_error<Stmt>( "Expected '{' after class name" );

  if( !match( RBRACE ) )
    return make_error<Stmt>( "Expected '}' after class declaration" );

  return make_result<Stmt>( m_arena.alloc<ClassDecl>( name.c_str() ) );
}

Result<Stmt> Parser::parse_block()
{
  Block * block = m_arena.alloc<Block>();
  do
  {
    if( match( RBRACE ) )
      break;

    auto stmt = parse_declaration();

    if( !stmt.ok() )
      return make_error<Stmt>( stmt.error );

    block->stmts.push_back( stmt.node );
  } while( !is_finished() );

  return make_result<Stmt>( block );
}

Result<Stmt> Parser::parse_expr_stmt()
{
  auto expr = parse_expression();

  if( !expr.ok() )
    return make_error<Stmt>( expr.error );

  if( !match( SEMICOLON ) )
    return make_error<Stmt>( "Expected ';' after expression" );

  return make_result<Stmt>( m_arena.alloc<ExprStmt>( expr.node ) );
}

Result<Stmt> Parser::parse_while()
{
  if( !match( LPAREN ) )
    return make_error<Stmt>( "expected '('" );

  auto cond = parse_expression();
  if( !cond.ok() )
    return make_error<Stmt>( cond.error );

  if( !match( RPAREN ) )
    return make_error<Stmt>( "expected ')'" );

  auto body = parse_declaration();
  if( !body.ok() )
    return make_error<Stmt>( body.error );

  WhileStmt * stmt = m_arena.alloc<WhileStmt>( cond.node, body.node );
  return make_result<Stmt>( stmt );
}

Result<Stmt> Parser::parse_if()
{
  if( !match( LPAREN ) )
    return make_error<Stmt>( "expected '('" );

  auto cond = parse_expression();
  if( !cond.ok() )
    return make_error<Stmt>( cond.error );

  if( !match( RPAREN ) )
    return make_error<Stmt>( "expected ')'" );

  Stmt * a = nullptr;
  Stmt * b = nullptr;

  auto then_branch = parse_declaration();
  if( !then_branch.ok() )
    return make_error<Stmt>( then_branch.error );

  a = then_branch.node;

  if( match( KW_ELSE ) )
  {
    auto else_branch = parse_declaration();
    if( !else_branch.ok() )
      return make_error<Stmt>( else_branch.error );

    b = else_branch.node;
  }

  IfStmt * if_stmt = m_arena.alloc<IfStmt>( cond.node, a, b );
  return make_result<Stmt>( if_stmt );
}

Result<Expr> Parser::parse_expression()
{
  return parse_assignment();
}

Result<Expr> Parser::parse_primary()
{
  if( match( NUMBER ) )
  {
    int value         = std::stoi( previous().lexeme );
    Literal * literal = m_arena.alloc<Literal>( Object::Integer( value ) );
    return make_result<Expr>( literal );
  }
  else if( match( STRING ) )
  {
    std::string str   = previous().lexeme;
    Literal * literal = m_arena.alloc<Literal>( Object::String( str.c_str() ) );
    return make_result<Expr>( literal );
  }
  else if( match( IDENTIFIER ) )
  {
    Variable * var = m_arena.alloc<Variable>( previous().lexeme );
    return make_result<Expr>( var );
  }
  else
  {
    return make_error<Expr>( "Not Implemented" );
  }
}

Result<Expr> Parser::parse_unary()
{
#if 0
  if( match( MINUS ) )
  {
    std::string op = previous().lexeme;

    auto right     = parse_unary();
    if( !right.ok() )
      return make_error<Expr>( right.error );

    return make_result<Expr>( right.node );
  }
#endif

  return parse_call();
}

Result<Expr> Parser::parse_term()
{
  auto res_expr = parse_factor();
  if( !res_expr.ok() )
    return make_error<Expr>( res_expr.error );

  Expr * expr = res_expr.node;

  while( match( PLUS ) || match( MINUS ) )
  {
    std::string op = previous().lexeme;

    auto right = parse_factor();
    if( !right.ok() )
      return make_error<Expr>( right.error );

    expr = m_arena.alloc<Binary>( op, expr, right.node );
  }

  return make_result( expr );
}

Result<Expr> Parser::parse_factor()
{
  auto res_expr = parse_unary();
  if( !res_expr.ok() )
    return make_error<Expr>( res_expr.error );

  Expr * expr = res_expr.node;

  while( match( STAR ) || match( SLASH ) )
  {
    std::string op = previous().lexeme;

    auto right = parse_unary();
    if( !right.ok() )
      return make_error<Expr>( right.error );

    expr = m_arena.alloc<Binary>( op, expr, right.node );
  }

  return make_result( expr );
}

const Token & Parser::previous()
{
  return *( m_pos - 1 );
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

bool Parser::is_finished()
{
  return m_pos == m_tokens.end();
}

Result<Program> parse( const std::vector<Token> & tokens, NodeAllocator & allocator )
{
  Parser parser( tokens, allocator );
  return parser.run();
}
