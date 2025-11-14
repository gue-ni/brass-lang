#include "ast.h"
#include <algorithm>
#include <cassert>
#include <iostream>

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
  // declare builtin functions
  ( void ) compiler.define_var( "typeof" );

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

  OpCode instr = OP_NOP;

  if( op == "+" )
  {
    instr = OP_ADD;
  }
  else if( op == "-" )
  {
    instr = OP_SUB;
  }
  else if( op == "*" )
  {
    instr = OP_MULT;
  }
  else if( op == "/" )
  {
    instr = OP_DIV;
  }
  else
  {
    assert( false && "Unreachable" );
  }

  compiler.code->emit_instr( instr );
}

Print::Print( Expr * expr, bool newline )
    : expr( expr )
    , newline( newline )
{
}

void Print::compile( Compiler & compiler )
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
  cond->compile( compiler );

  size_t jmp_1, jmp_2;

  jmp_1 = compiler.code->emit_jump( OP_JMP_IF_FALSE );

  then_stmt->compile( compiler );

  if( else_stmt )
  {
    jmp_2 = compiler.code->emit_jump( OP_JMP );
  }

  compiler.code->end_jump( jmp_1 );

  if( else_stmt )
  {
    else_stmt->compile( compiler );
    compiler.code->end_jump( jmp_2 );
  }
}

WhileStmt::WhileStmt( Expr * cond, Stmt * body )
    : cond( cond )
    , body( body )
{
}

void WhileStmt::compile( Compiler & compiler )
{
  size_t jmp_2 = compiler.code->instructions.size();
  cond->compile( compiler );
  size_t jmp_1 = compiler.code->emit_jump( OP_JMP_IF_FALSE );
  body->compile( compiler );
  compiler.code->emit_loop( jmp_2 );
  compiler.code->end_jump( jmp_1 );
}

FnDecl::FnDecl( const std::string & name, const std::vector<std::string> & args, Stmt * body )
    : name( name )
    , args( args )
    , body( body )
{
}

void FnDecl::compile( Compiler & compiler )
{

  CodeObject * global = compiler.code;

  FunctionObject * fn = compiler.gc.alloc<FunctionObject>( name.c_str(), ( uint8_t ) args.size(), global );

  uint16_t index = compiler.define_var( name );
  compiler.code->emit_literal( Object::Function( fn ) );
  compiler.code->emit_instr( OP_STORE_GLOBAL, index );

  compiler.code = &fn->code_object;
  compiler.push_scope();

  for( const std::string & arg : args )
  {
    uint16_t tmp = compiler.define_var( arg );
  }

  body->compile( compiler );

  compiler.pop_scope();
  compiler.code = global;
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
  if( index == UNDEFINED )
  {
    std::cerr << "Undefined varaible " << name << std::endl;
  }
  compiler.code->emit_instr( is_global ? OP_LOAD_GLOBAL : OP_LOAD_LOCAL, index );
}

Call::Call( Expr * callee, const std::vector<Expr *> & args )
    : callee( callee )
    , args( args )
{
}

void Call::compile( Compiler & compiler )
{
  for( Expr * expr : args )
  {
    expr->compile( compiler );
  }
  callee->compile( compiler );
  compiler.code->emit_instr( OP_CALL, ( uint16_t ) args.size() );
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
  uint16_t index = compiler.define_var( name );
  bool global    = compiler.scopes.size() == 1;
  compiler.code->emit_instr( global ? OP_STORE_GLOBAL : OP_STORE_LOCAL, index );
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
  compiler.code->emit_instr( is_global ? OP_STORE_GLOBAL : OP_STORE_LOCAL, index );
}

ClassDecl::ClassDecl( const std::string & name )
    : name( name )
{
}

void ClassDecl::compile( Compiler & compiler )
{
  ClassObject * cls = compiler.gc.alloc<ClassObject>( name.c_str() );
  uint16_t index    = compiler.define_var( name );
  compiler.code->emit_literal( Object::Class( cls ) );
  compiler.code->emit_instr( OP_STORE_GLOBAL, index );
}

Get::Get( Expr * object, const std::string & name )
    : object( object )
    , property( name )
{
}

void Get::compile( Compiler & compiler )
{
  object->compile( compiler );
  uint16_t index = compiler.define_global_var( property );
  compiler.code->emit_instr( OP_GET_PROPERTY, index );
}

Set::Set( Expr * object, const std::string & name, Expr * value )
    : object( object )
    , property( name )
    , value( value )
{
}

void Set::compile( Compiler & compiler )
{
  value->compile( compiler );
  object->compile( compiler );
  uint16_t index = compiler.define_global_var( property );
  compiler.code->emit_instr( OP_SET_PROPERTY, index );
}

ExprStmt::ExprStmt( Expr * expr )
    : expr( expr )
{
}

void ExprStmt::compile( Compiler & compiler )
{
  expr->compile( compiler );
}
