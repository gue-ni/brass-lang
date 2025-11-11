#pragma once

#include <vector>

#include "compiler.h"
#include "object.h"

struct AstNode
{
  virtual ~AstNode() {}
  virtual void compile( Compiler & );
};

struct Expr : AstNode
{
};

struct Stmt : AstNode
{
};

struct Literal : Expr
{
  Object value;
  Literal( Object value );
  void compile( Compiler & ) override;
};

struct Binary : Expr
{
  std::string op;
  Expr * rhs;
  Expr * lhs;
  Binary( const std::string & op, Expr * lhs, Expr * rhs );
  void compile( Compiler & compiler ) override;
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
};

struct VariableDecl : Stmt
{
  std::string name;
  Expr * expr;
  VariableDecl( const std::string & name, Expr * expr );
  void compile( Compiler & compiler ) override;
};

struct Assignment : Stmt
{
  std::string name;
  Expr * expr;
  Assignment( const std::string & name, Expr * expr );
  void compile( Compiler & compiler ) override;
};

struct Program : Stmt
{
  std::vector<Stmt *> stmts;
  void compile( Compiler & compiler ) override;
};

struct Block : Stmt
{
  std::vector<Stmt *> stmts;
  void compile( Compiler & compiler ) override;
};

struct FnDecl : Stmt
{
  std::string name;
  std::vector<std::string> args;
  Stmt * body;
  FnDecl( const std::string & name, const std::vector<std::string> & args, Stmt * body );
  void compile( Compiler & compiler ) override;
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

struct DebugPrint : Stmt
{
  bool newline;
  Expr * expr;
  DebugPrint( Expr * expr, bool newline = false );
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
};

struct Get : Expr
{
  std::string name;
  Expr * object;
  Get( Expr * object, const std::string & name );
  void compile( Compiler & compiler ) override;
};


// basic allocator, should be replaced by a arena allocator
// arena allocator does not allow me to use stl containers
class NodeAllocator
{
  public:
  NodeAllocator() {}
  ~NodeAllocator() {
    for (AstNode* node : m_nodes)
    {
      delete node;
    }
    m_nodes.clear();
  }

  template <typename T, typename... Args>
  T * alloc( Args &&... args )
  {
    T* ptr = new T( std::forward<Args>( args )... );
    m_nodes.push_back(ptr);
    return ptr;
  }

private:
  std::list<AstNode*> m_nodes;
};

