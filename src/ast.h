#pragma once

#include <vector>

#include "compiler.h"
#include "object.h"

struct TypeInfo
{
  std::string name;
  bool declard = false;

  std::map<std::string, TypeInfo *> field_types;

  // only needed for functions
  TypeInfo * return_type;
  std::vector<TypeInfo *> arg_types;

  bool is_callable() const
  {
    return return_type != nullptr;
  }

  TypeInfo( const std::string & info )
      : name( info )
      , return_type( nullptr )
  {
  }
};

class TypeContext
{
public:
  TypeContext();
  ~TypeContext();
  void push_scope();
  void pop_scope();
  void define_var( const std::string & name, TypeInfo * type_info );
  TypeInfo * lookup_var( const std::string & name );
  TypeInfo * define_type( const std::string & name );
  TypeInfo * lookup_type( const std::string & name );

  void throw_type_error( const std::string & msg );

  bool ok() const
  {
    return error.empty();
  }

  std::string error;
  TypeInfo * expected_return_type = nullptr;

private:
  // mapping of type names to TypeInfo*
  std::map<std::string, TypeInfo *> m_types;

  // mapping of variable names to types
  std::list<std::map<std::string, TypeInfo *>> m_scopes;
};

struct AstNode : public GarbageCollected
{
  virtual ~AstNode()
  {
  }

  virtual void compile( Compiler & ) = 0;
};

struct Expr : AstNode
{
  virtual TypeInfo * infer_types( TypeContext & ctx ) = 0;
};

struct Stmt : AstNode
{
  virtual bool declare_global( TypeContext & ctx );
  virtual bool check_types( TypeContext & ctx ) = 0;
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
  TypeInfo * infer_types( TypeContext & ctx ) override;
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
  std::string var_name;
  std::string type_name;
  Expr * expr;
  VariableDecl( const std::string & var_name, const std::string & type_name, Expr * expr );
  void compile( Compiler & compiler ) override;
  bool check_types( TypeContext & ctx ) override;
  bool declare_global( TypeContext & ctx ) override;
};

struct Assignment : Expr
{
  std::string name;
  Expr * expr;
  Assignment( const std::string & name, Expr * expr );
  void compile( Compiler & compiler ) override;
  TypeInfo * infer_types( TypeContext & ctx ) override;
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

struct FnArgDecl
{
  std::string name;
  std::string type;
};

struct FnDecl : Stmt
{
  std::string name;
  std::vector<FnArgDecl> args;
  std::string return_type;
  Stmt * body;
  FnDecl( const std::string & name, const std::vector<FnArgDecl> & args, const std::string & return_type, Stmt * body );
  void compile( Compiler & compiler ) override;
  bool declare_global( TypeContext & ctx ) override;
  bool check_types( TypeContext & ctx ) override;
};

struct IfStmt : Stmt
{
  Expr * cond;
  Stmt * then_stmt;
  Stmt * else_stmt;
  IfStmt( Expr * cond, Stmt * then, Stmt * otherwise );
  void compile( Compiler & compiler ) override;
  bool check_types( TypeContext & ctx ) override;
};

struct WhileStmt : Stmt
{
  Expr * cond;
  Stmt * body;
  WhileStmt( Expr * cond, Stmt * body );
  void compile( Compiler & compiler ) override;
  bool check_types( TypeContext & ctx ) override;
};

struct Print : Stmt
{
  bool newline;
  Expr * expr;
  Print( Expr * expr, bool newline = false );
  void compile( Compiler & compiler ) override;
  bool check_types( TypeContext & ctx ) override;
};

struct Return : Stmt
{
  Expr * expr;
  Return( Expr * expr );
  void compile( Compiler & compiler ) override;
  bool check_types( TypeContext & ctx ) override;
};

struct ClassFieldDecl
{
  std::string name;
  std::string type;
};

struct ClassDecl : Stmt
{
  std::string name;
  std::vector<ClassFieldDecl> fields;
  ClassDecl( const std::string & name );
  ClassDecl( const std::string & name, const std::vector<ClassFieldDecl> & fields );
  void compile( Compiler & compiler ) override;
  bool declare_global( TypeContext & ctx ) override;
  bool check_types( TypeContext & ctx ) override;
};

struct Get : Expr
{
  Expr * object;
  std::string property;
  Get( Expr * object, const std::string & name );
  void compile( Compiler & compiler ) override;
  TypeInfo * infer_types( TypeContext & ctx ) override;
};

struct Set : Expr
{
  Expr * object;
  std::string property;
  Expr * value;
  Set( Expr * object, const std::string & name, Expr * value );
  void compile( Compiler & compiler ) override;
  TypeInfo * infer_types( TypeContext & ctx ) override;
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
