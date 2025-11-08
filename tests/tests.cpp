#include <gtest/gtest.h>

#include "allocator.h"
#include "ast.h"
#include "brass.h"
#include "gc.h"
#include "object.h"
#include "vm.h"

class Unittest : public ::testing::Test
{
public:

protected:
  std::ostringstream out, err;
};

TEST_F( Unittest, test_000 )
{
  const char * src = R"(
print(1 + 2);
  )";
  int r            = eval( src, out, err );
  ASSERT_EQ( r, 0 );
  ASSERT_EQ( out.str(), "" );
  ASSERT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_001 )
{
  CodeObject code;
  code.emit_literal( Value( 2 ) );
  code.emit_literal( Value( 3 ) );
  code.emit_instr( OP_BINARY_ADD );
  code.emit_instr( OP_DEBUG_PRINT );

  GarbageCollector gc;
  VirtualMachine vm( out, err, gc );

  int r = vm.run( &code );

  ASSERT_EQ( r, 0 );
  ASSERT_EQ( out.str(), "5" );
  ASSERT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_002 )
{
  ArenaAllocator allocator( 1024 );
  Program * prog = allocator.alloc<Program>();

  Stmt * stmt_1 = allocator.alloc<DebugPrint>( allocator.alloc<Literal>( Value( 42 ) ) );

  prog->stmts.push_back( stmt_1 );

  Stmt * stmt_2 = allocator.alloc<DebugPrint>(
      allocator.alloc<Binary>( allocator.alloc<Literal>( Value( 2 ) ), allocator.alloc<Literal>( Value( 3 ) ) ) );

  prog->stmts.push_back( stmt_2 );

  CodeObject code = compile( prog );

  GarbageCollector gc;
  VirtualMachine vm( out, err, gc );

  int r = vm.run( &code );

  ASSERT_EQ( r, 0 );
  ASSERT_EQ( out.str(), "425" );
  ASSERT_EQ( err.str(), "" );

}
