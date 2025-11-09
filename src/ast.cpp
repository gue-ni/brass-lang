#include "ast.h"
#include <algorithm>
#include <cassert>

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

Binary::Binary( const std::string & op, Expr * lhs, Expr * rhs )
    : op( op )
    , lhs( lhs )
    , rhs( rhs )
{
}

void Binary::compile( Compiler & compiler )
{
  rhs->compile( compiler );
  lhs->compile( compiler );

  Instruction instr = OP_NOP;

  if( op == "+" )
  {
    instr = OP_ADD;
  }
  else if( op == "-" )
  {
    instr = OP_SUB;
  }
  else
  {
    assert( false && "Unreachable" );
  }

  compiler.code->emit_instr( instr );
}

DebugPrint::DebugPrint( Expr * expr, bool newline )
    : expr( expr )
    , newline( newline )
{
}

void DebugPrint::compile( Compiler & compiler )
{
  expr->compile( compiler );
  compiler.code->emit_instr( newline ? OP_PRINTLN : OP_PRINT );
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

  CodeObject * context = compiler.code;
  compiler.code        = &fn->code_object;
  compiler.push_scope();

  for( const std::string & arg : args )
  {
    uint16_t tmp = compiler.define_var( arg );
  }

  body->compile( compiler );

  compiler.pop_scope();
  compiler.code = context;

  uint16_t index = compiler.define_var( name );
  compiler.code->emit_literal( Object::Function( fn ) );
  compiler.code->emit_instr( OP_MAKE_FUNCTION );
  compiler.code->emit_instr( OP_STORE_GLOBAL, index );
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
  auto [index, is_global] = compiler.find_var( name );
  if( is_global )
  {
    compiler.code->emit_instr( OP_LOAD_GLOBAL, index );
  }
  else
  {
    compiler.code->emit_instr( OP_LOAD_LOCAL, index );
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

  if( compiler.scopes.size() == 1 )
  {
    uint16_t index = compiler.define_var( name );
    compiler.code->emit_instr( OP_STORE_GLOBAL, index );
  }
  else
  {
    uint16_t index = compiler.define_var( name );
    compiler.code->emit_instr( OP_STORE_LOCAL, index );
  }
}

Assignment::Assignment( const std::string & name, Expr * expr )
    : name( name )
    , expr( expr )
{
}

void Assignment::compile( Compiler & compiler )
{
  expr->compile( compiler );

  auto [index, is_global] = compiler.find_var( name );
  if( is_global )
  {
    compiler.code->emit_instr( OP_STORE_GLOBAL, index );
  }
  else
  {
    compiler.code->emit_instr( OP_STORE_LOCAL, index );
  }
}
