#include "ast.h"
#include <algorithm> 

void AstNode::compile( Compiler & )
{
}

Literal::Literal( Object value )
    : value( value )
{
}

void Literal::compile( Compiler & compiler )
{
  compiler.code->emit_literal( value );
}

void Program::compile( Compiler & compiler )
{
  for( Stmt * stmt : stmts )
  {
    stmt->compile( compiler );
  }
}

Binary::Binary( Expr * lhs, Expr * rhs )
    : lhs( lhs )
    , rhs( rhs )
{
}

void Binary::compile( Compiler & compiler )
{
  rhs->compile( compiler );
  lhs->compile( compiler );
  compiler.code->emit_instr( OP_ADD );
}

DebugPrint::DebugPrint( Expr * expr )
    : expr( expr )
{
}

void DebugPrint::compile( Compiler & compiler )
{
  expr->compile( compiler );
  compiler.code->emit_instr( OP_DEBUG_PRINT );
}

IfStmt::IfStmt( Expr * cond, Stmt * then_stmt, Stmt * else_stmt )
    : cond( cond )
    , then_stmt( then_stmt )
    , else_stmt( else_stmt )
{
}

void IfStmt::compile( Compiler & compiler )
{
  // TODO
}

WhileStmt::WhileStmt( Expr * cond, Stmt * body )
    : cond( cond )
    , body( body )
{
}

void WhileStmt::compile( Compiler & compiler )
{
  // TODO
}

FnDecl::FnDecl( const std::string & name, const std::vector<std::string> & args, Stmt * body )
    : name( name )
    , args( args )
    , body( body )
{
}

void FnDecl::compile( Compiler & compiler )
{
  FunctionObject * fn = compiler.gc.alloc<FunctionObject>( name.c_str(), ( uint8_t ) args.size() );

  CodeObject * tmp = compiler.code;
  compiler.code    = &fn->code_object;

  compiler.push_scope();

  for( const std::string & arg : args )
  {
    uint16_t tmp = compiler.define_local( arg );
  }

  body->compile( compiler );

  //fn->num_locals = compiler.num_locals;

  compiler.pop_scope();

  compiler.code = tmp;

  auto var = compiler.code->emit_name( name );
  compiler.code->emit_literal( Object::Function( fn ) );
  compiler.code->emit_instr( OP_MAKE_FUNCTION );
  compiler.code->emit_instr( OP_STORE_GLOBAL, var );
}

Return::Return( Expr * expr )
    : expr( expr )
{
}

void Return::compile( Compiler & compiler )
{
  expr->compile( compiler );
  compiler.code->emit_instr( OP_RETURN );
}

Variable::Variable( const std::string & name )
    : name( name )
{
}

void Variable::compile( Compiler & compiler )
{
  if( compiler.scopes.empty() )
  {
    auto it = std::find( compiler.code->names.begin(), compiler.code->names.end(), name );
    if( it != compiler.code->names.end() )
    {
      size_t index = std::distance( compiler.code->names.begin(), it );
      compiler.code->emit_instr( OP_LOAD_GLOBAL,  index );
    }
  }
  else
  {
    uint16_t index = compiler.define_local(name);
    compiler.code->emit_instr(OP_LOAD_LOCAL, index);
  }
}

FnCall::FnCall( Expr * callee, const std::vector<Expr *> & args )
    : callee( callee )
    , args( args )
{
}

void FnCall::compile( Compiler & compiler )
{
  for( Expr * expr : args )
  {
    expr->compile( compiler );
  }
  callee->compile( compiler );
  compiler.code->emit_instr( OP_CALL_FUNCTION );
}

void Block::compile( Compiler & compiler )
{
  compiler.push_scope();
  for( Stmt * stmt : stmts )
  {
    stmt->compile( compiler );
  }
  compiler.pop_scope();
}

VariableDecl::VariableDecl( const std::string & name, Expr * expr )
    : name( name )
    , expr( expr )
{
}

void VariableDecl::compile( Compiler & compiler )
{
  expr->compile( compiler );

  if( compiler.scopes.empty() )
  {
    uint16_t index = compiler.code->emit_name( name );
    compiler.code->emit_instr( OP_STORE_GLOBAL, index );
  }
  else
  {
    uint16_t index = compiler.define_local( name );
    compiler.code->emit_instr(OP_STORE_LOCAL, index);
  }
}

Assignment::Assignment( const std::string & name, Expr * expr )
    : name( name )
    , expr( expr )
{
}

void Assignment::compile( Compiler & compiler )
{
  auto it = std::find( compiler.code->names.begin(), compiler.code->names.end(), name );
  if( it != compiler.code->names.end() )
  {
    expr->compile( compiler );

    size_t index = std::distance( compiler.code->names.begin(), it );
    compiler.code->emit_instr( OP_LOAD_GLOBAL, ( uint8_t ) index );
  }
}
