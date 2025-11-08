#pragma once

#include <vector>

#include "compiler.h"
#include "object.h"

struct AstNode
{
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
  Value value;
  Literal( Value value );
  void compile( Compiler & ) override;
};

struct Binary : Expr
{
  Expr * rhs;
  Expr * lhs;
  Binary( Expr * lhs, Expr * rhs );
  void compile( Compiler & compiler ) override;
};

struct FnCall : Expr
{
};

struct Program : Stmt
{
  std::vector<Stmt *> stmts;
  void compile( Compiler & compiler ) override;
};

struct FnDecl : Stmt
{
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
  Expr * expr;
  DebugPrint( Expr * expr );
  void compile( Compiler & compiler ) override;
};
