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
        return ctx.lookup_type( "int" );
      }
    case Object ::Type ::STRING :
      {
        return ctx.lookup_type( "string" );
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
    if( !stmt->declare_global( ctx ) )
    {
      return false;
    }
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

FnDecl::FnDecl(
    const std::string & name, const std::vector<FnArgDecl> & args, const std::string & return_type, Stmt * body )
    : name( name )
    , args( args )
    , return_type( return_type )
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

  for( const auto & arg : args )
  {
    uint16_t tmp = compiler.define_var( arg.name );
  }

  body->compile( compiler );

  compiler.pop_scope();
  compiler.code = global;
}

bool FnDecl::declare_global( TypeContext & ctx )
{
  TypeInfo * retval = ctx.lookup_type( return_type ); 
  std::string type_name;
  std::vector<TypeInfo *> arg_types;

  for( const auto & arg : args )
  {
    TypeInfo * tmp = ctx.lookup_type( arg.type );
    arg_types.push_back( tmp );

    if( !type_name.empty() )
      type_name += ", ";

    type_name += tmp->name;
  }

  type_name = "(" + type_name + ") -> " + retval->name;

  TypeInfo * fn_type   = ctx.define_type( type_name );
  fn_type->arg_types   = arg_types;
  fn_type->return_type = retval;

  ctx.define_var( name, fn_type );
  return true;
}

bool FnDecl::check_types( TypeContext & ctx )
{
  return true;
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
  TypeInfo * fn_type = callee->infer_types( ctx );
  if( !fn_type )
  {
    ctx.throw_type_error( "Undeclared function type" );
    return nullptr;
  }

  if( fn_type->arg_types.size() != args.size() )
  {
    ctx.throw_type_error( "Arity mismatch in function call" );
    return nullptr;
  }

  for( size_t i = 0; i < args.size(); i++ )
  {
    TypeInfo * arg_type = args[i]->infer_types( ctx );
    if( arg_type != fn_type->arg_types[i] )
    {
      ctx.throw_type_error(
          "Invalid argument of type '" + arg_type->name + "', expected '" + fn_type->arg_types[i]->name + "'" );
      return nullptr;
    }
  }

  return fn_type->return_type;
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

VariableDecl::VariableDecl( const std::string & var_name, const std::string & type_name, Expr * expr )
    : var_name( var_name )
    , type_name( type_name )
    , expr( expr )
{
}

void VariableDecl::compile( Compiler & compiler )
{
  expr->compile( compiler );
  uint16_t index = compiler.define_var( var_name );
  bool global    = compiler.scopes.size() == 1;
  compiler.code->emit_instr( global ? OP_STORE_GLOBAL : OP_STORE_LOCAL, index );
}

bool VariableDecl::check_types( TypeContext & ctx )
{
  // TODO: check if the variable was declard before this should throw an error
  TypeInfo * a = ctx.lookup_var( var_name ); // TODO: should also return whether it is global
  if( a->declard )
  {
    ctx.throw_type_error( "was already declared" );
  }
  else
  {
    a->declard = true;
  }

  TypeInfo * decl_type = nullptr;
  if( !type_name.empty() )
  {
    decl_type = ctx.lookup_type( type_name );
  }

  TypeInfo * infered_type = expr->infer_types( ctx );

  if( decl_type && decl_type != infered_type )
  {
    ctx.throw_type_error( "Type mismatch in variable declaration" );
    return false;
  }
  else
  {
    ctx.define_var( var_name, infered_type );
    return true;
  }
}

bool VariableDecl::declare_global( TypeContext & ctx )
{
  TypeInfo * expr_type = expr->infer_types( ctx );

  TypeInfo * decl_type = nullptr;

  if( !type_name.empty() )
  {
    decl_type = ctx.lookup_type( type_name );
  }

  if( decl_type && expr_type != decl_type )
  {
    ctx.throw_type_error( "Declared type does not match infered type" );
    return false;
  }

  if( expr_type )
  {
    ctx.define_var( var_name, expr_type );
  }
  return true;
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

bool ClassDecl::declare_global( TypeContext & ctx )
{
  TypeInfo * type_info = ctx.define_type( name );
  ctx.define_var( name, type_info );


  std::string ctor_type_name = "() -> " + name;

  TypeInfo* ctor_type = ctx.define_type(ctor_type_name);
  ctor_type->arg_types = {};
  ctor_type->return_type = type_info;

  ctx.define_var(name, ctor_type);
  return true;
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
  // builtin types
  ( void ) define_type( "int" );
  ( void ) define_type( "string" );
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
  auto it = m_types.find( name );
  if( it != m_types.end() )
  {
    return it->second;
  }
  else
  {
    return nullptr;
  }
}

void TypeContext::throw_type_error( const std::string & msg )
{
  error = msg;
}

bool Stmt::declare_global( TypeContext & ctx )
{
  // do nothing
  return true;
}
