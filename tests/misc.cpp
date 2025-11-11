#include <gtest/gtest.h>

#include "object.h"
#include "utils.h"
#include "allocator.h"
#include "ast.h"

TEST(misc, test_alloc_00)
{
  NodeAllocator allocator;
  auto* ast = allocator.alloc<DebugPrint>(allocator.alloc<Literal>(Object::Integer(5)));
}

TEST(misc, test_alloc_01)
{
  NodeAllocator allocator;
  Program* prog = allocator.alloc<Program>();
  prog->stmts.push_back(allocator.alloc<DebugPrint>(allocator.alloc<Literal>(Object::Integer(5))));
}