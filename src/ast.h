#pragma once

#include <vector>

struct AstNode
{
};

struct Expr : AstNode
{
};

struct Stmt : AstNode
{
};

struct Program : Stmt
{
  std::vector<Stmt*> stmts;
};