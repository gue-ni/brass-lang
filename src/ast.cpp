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

TypeInfo * Literal::infer_types( TypeContext & ctx )
{
  switch( value.type )
  {
    case Object ::Type ::INTEGER :
      {
        return ctx.define_type( "integer" );
      }
    case Object ::Type ::STRING :
      {
        return ctx.define_type( "string" );
      }
    default :
      assert( false );
      return nullptr;
  }
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

bool Program::check_types( TypeContext & ctx )
{
  ctx.push_scope();
  for( Stmt * stmt : stmts )
  {
    stmt->declare_global( ctx ); // if
  }

  for( Stmt * stmt : stmts )
  {
    if( !stmt->check_types( ctx ) )
    {
      return false;
    }
  }
  ctx.pop_scope();
  return true;
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

TypeInfo * Binary::infer_types( TypeContext & ctx )
{
  TypeInfo * l = lhs->infer_types( ctx );
  TypeInfo * r = rhs->infer_types( ctx );
  if( l == r )
  {
    return r;
  }
  else
  {
    ctx.throw_type_error( "Type mismatch in binary operation" );
    return nullptr;
  }
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

bool Print::check_types( TypeContext & ctx )
{
  TypeInfo * ti = expr->infer_types( ctx );
  return ti != nullptr;
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

bool IfStmt::check_types( TypeContext & ctx )
{
  return false;
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

bool WhileStmt::check_types( TypeContext & ctx )
{
  return false;
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

void FnDecl::declare_global( TypeContext & ctx )
{
}

bool FnDecl::check_types( TypeContext & ctx )
{
  return false;
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

bool Return::check_types( TypeContext & ctx )
{
  // TODO: check if type matches function return value
  TypeInfo * ti = expr->infer_types( ctx );
  return ti != nullptr;
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

TypeInfo * Variable::infer_types( TypeContext & ctx )
{
  TypeInfo * ti = ctx.lookup_var( name );
  return ti;
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

TypeInfo * Call::infer_types( TypeContext & ctx )
{
  TypeInfo * ti = callee->infer_types( ctx );
  return ti;
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

bool Block::check_types( TypeContext & ctx )
{
  return false;
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

bool VariableDecl::check_types( TypeContext & ctx )
{
  // TODO: check if the variable was declard before this should throw an error
  TypeInfo * a = ctx.lookup_var( name ); // TODO: should also return whether it is global

  if( a->declard )
  {
    ctx.throw_type_error( "was already declared" );
  }
  else
  {
    a->declard = true;
  }

  TypeInfo * b = expr->infer_types( ctx );

  if( a != b )
  {
    ctx.throw_type_error( "balksdf" );
    return false;
  }
  else
  {
    return true;
  }
}

void VariableDecl::declare_global( TypeContext & ctx )
{
  TypeInfo * expr_type = expr->infer_types( ctx );
  if( expr_type )
  {
    ctx.define_var( name, expr_type );
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
  compiler.code->emit_instr( is_global ? OP_STORE_GLOBAL : OP_STORE_LOCAL, index );
}

TypeInfo * Assignment::infer_types( TypeContext & ctx )
{
  TypeInfo * var_type = ctx.lookup_var( name );

  if( !var_type )
  {
    ctx.throw_type_error( "undeclard variable assigned" );
    return nullptr;
  }

  TypeInfo * expr_type = expr->infer_types( ctx );

  if( var_type != expr_type )
  {
    ctx.throw_type_error( "Type mismatch in assignment" );
    return nullptr;
  }

  return expr_type;
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

void ClassDecl::declare_global( TypeContext & ctx )
{
  TypeInfo * type_info = ctx.define_type( name );
  ctx.define_var( name, type_info );
}

bool ClassDecl::check_types( TypeContext & ctx )
{
  return true;
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

TypeInfo * Get::infer_types( TypeContext & ctx )
{
  return nullptr;
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

TypeInfo * Set::infer_types( TypeContext & ctx )
{
  return nullptr;
}

ExprStmt::ExprStmt( Expr * expr )
    : expr( expr )
{
}

void ExprStmt::compile( Compiler & compiler )
{
  expr->compile( compiler );
}

bool ExprStmt::check_types( TypeContext & ctx )
{
  ( void ) expr->infer_types( ctx );
  return true;
}

TypeContext::TypeContext()
{
}

void TypeContext::push_scope()
{
  m_scopes.push_back( {} );
}

void TypeContext::pop_scope()
{
  m_scopes.pop_back();
}

void TypeContext::define_var( const std::string & name, TypeInfo * type_info )
{
  auto & scope = m_scopes.back();
  scope[name]  = type_info;
}

TypeInfo * TypeContext::lookup_var( const std::string & name )
{
  for( auto scope_it = m_scopes.rbegin(); scope_it != m_scopes.rend(); scope_it++ )
  {
    auto it = scope_it->find( name );
    if( it != scope_it->end() )
    {
      return it->second;
    }
  }
  return nullptr;
}

TypeInfo * TypeContext::define_type( const std::string & name )
{
  auto it = m_types.find( name );
  if( it != m_types.end() )
  {
    return it->second;
  }
  else
  {
    TypeInfo * ti = new TypeInfo( name );
    m_types[name] = ti;
    return ti;
  }
}

TypeInfo * TypeContext::lookup_type( const std::string & name )
{
  return nullptr;
}

void TypeContext::throw_type_error( const std::string & msg )
{
  error = msg;
}
