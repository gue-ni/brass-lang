#pragma once

#include <vector>

#include "compiler.h"
#include "object.h"

struct TypeInfo
{
  std::string type_name;
  bool declard = false;
  TypeInfo( const std::string & info )
      : type_name( info )
  {
  }
};

class TypeContext
{
public:
  TypeContext();
  void push_scope();
  void pop_scope();
  void define_var( const std::string & name, TypeInfo * type_info );
  TypeInfo * lookup_var( const std::string & name );
  TypeInfo * define_type( const std::string & name, bool forward = false );
  TypeInfo * lookup_type( const std::string & name );

  void throw_type_error( const std::string & msg );

  bool ok() const
  {
    return error.empty();
  }

  // TypeInfo*
  std::string error;

private:
  std::map<std::string, TypeInfo *> m_types; // mapping of type names to TypeInfo*

  std::list<std::map<std::string, TypeInfo *>> m_scopes; // mapping of variable names to types
};

struct AstNode
{
  virtual ~AstNode()
  {
  }

  virtual void compile( Compiler & );

  virtual bool check_types( TypeContext & ctx )
  {
    return true;
  }
};

struct Expr : AstNode
{
  TypeInfo * type = nullptr;
  virtual TypeInfo * infer_types( TypeContext & ctx )
  {
    return nullptr;
  }
};

struct Stmt : AstNode
{
  virtual void declare_global( TypeContext & ctx )
  {
    // do nothing
  }
};

struct Literal : Expr
{
  Object value;
  Literal( Object value );
  void compile( Compiler & ) override;
  TypeInfo * infer_types( TypeContext & ctx ) override;
};

struct Binary : Expr
{
  std::string op;
  Expr * rhs;
  Expr * lhs;
  Binary( const std::string & op, Expr * lhs, Expr * rhs );
  void compile( Compiler & compiler ) override;
  TypeInfo * infer_types( TypeContext & ctx ) override;
};

struct Call : Expr
{
  Expr * callee;
  std::vector<Expr *> args;
  Call( Expr * callee, const std::vector<Expr *> & args );
  void compile( Compiler & compiler ) override;
};

struct Variable : Expr
{
  std::string name;
  Variable( const std::string & name );
  void compile( Compiler & compiler ) override;
  TypeInfo * infer_types( TypeContext & ctx ) override;
};

struct ExprStmt : Stmt
{
  Expr * expr;
  ExprStmt( Expr * expr );
  void compile( Compiler & compiler ) override;
  bool check_types( TypeContext & ctx ) override;
};

struct VariableDecl : Stmt
{
  std::string name;
  Expr * expr;
  VariableDecl( const std::string & name, Expr * expr );
  void compile( Compiler & compiler ) override;
  bool check_types( TypeContext & ctx ) override;
  void declare_global( TypeContext & ctx ) override;
};

struct Assignment : Expr
{
  std::string name;
  Expr * expr;
  Assignment( const std::string & name, Expr * expr );
  void compile( Compiler & compiler ) override;
  TypeInfo * infer_types( TypeContext & ctx ) override;
  bool check_types( TypeContext & ctx ) override;
};

struct Program : Stmt
{
  std::vector<Stmt *> stmts;
  void compile( Compiler & compiler ) override;
  bool check_types( TypeContext & ctx ) override;
};

struct Block : Stmt
{
  std::vector<Stmt *> stmts;
  void compile( Compiler & compiler ) override;
  bool check_types( TypeContext & ctx ) override;
};

struct FnDecl : Stmt
{
  std::string name;
  std::vector<std::string> args;
  Stmt * body;
  FnDecl( const std::string & name, const std::vector<std::string> & args, Stmt * body );
  void compile( Compiler & compiler ) override;
  void declare_global( TypeContext & ctx ) override;
};

struct IfStmt : Stmt
{
  Expr * cond;
  Stmt * then_stmt;
  Stmt * else_stmt;
  IfStmt( Expr * cond, Stmt * then, Stmt * otherwise );
  void compile( Compiler & compiler ) override;
};

struct WhileStmt : Stmt
{
  Expr * cond;
  Stmt * body;
  WhileStmt( Expr * cond, Stmt * body );
  void compile( Compiler & compiler ) override;
};

struct Print : Stmt
{
  bool newline;
  Expr * expr;
  Print( Expr * expr, bool newline = false );
  void compile( Compiler & compiler ) override;
};

struct Return : Stmt
{
  Expr * expr;
  Return( Expr * expr );
  void compile( Compiler & compiler ) override;
};

struct ClassDecl : Stmt
{
  std::string name;
  ClassDecl( const std::string & name );
  void compile( Compiler & compiler ) override;
  void declare_global( TypeContext & ctx ) override;
};

struct Get : Expr
{
  Expr * object;
  std::string property;
  Get( Expr * object, const std::string & name );
  void compile( Compiler & compiler ) override;
};

struct Set : Expr
{
  Expr * object;
  std::string property;
  Expr * value;
  Set( Expr * object, const std::string & name, Expr * value );
  void compile( Compiler & compiler ) override;
};

// basic allocator, should be replaced by a arena allocator
// arena allocator does not allow me to use stl containers
class NodeAllocator
{
public:
  NodeAllocator()
  {
  }
  ~NodeAllocator()
  {
    for( AstNode * node : m_nodes )
    {
      delete node;
    }
    m_nodes.clear();
  }

  template <typename T, typename... Args>
  T * alloc( Args &&... args )
  {
    T * ptr = new T( std::forward<Args>( args )... );
    m_nodes.push_back( ptr );
    return ptr;
  }

private:
  std::list<AstNode *> m_nodes;
};
