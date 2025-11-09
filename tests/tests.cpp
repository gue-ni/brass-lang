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
print(2 + 3);
  )";
  int r            = eval( src, out, err );
  ASSERT_EQ( r, 0 );
  ASSERT_EQ( out.str(), "5" );
  ASSERT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_001 )
{
  CodeObject code;
  code.emit_literal( Object::Integer( 2 ) );
  code.emit_literal( Object::Integer( 3 ) );
  code.emit_instr( OP_ADD );
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

  Stmt * stmt_1 = allocator.alloc<DebugPrint>( allocator.alloc<Literal>( Object::Integer( 42 ) ) );

  prog->stmts.push_back( stmt_1 );

  Stmt * stmt_2 = allocator.alloc<DebugPrint>( allocator.alloc<Binary>(
      allocator.alloc<Literal>( Object::Integer( 2 ) ), allocator.alloc<Literal>( Object::Integer( 3 ) ) ) );

  prog->stmts.push_back( stmt_2 );

  GarbageCollector gc;

  CodeObject code = compile( prog, gc );

  VirtualMachine vm( out, err, gc );

  int r = vm.run( &code );

  ASSERT_EQ( r, 0 );
  ASSERT_EQ( out.str(), "425" );
  ASSERT_EQ( err.str(), "" );
}

#if 0
TEST_F( Unittest, test_003 )
{
  const char * src = R"(
fn add(a, b) {
  return a + b;
}

x = add(2, 3);

print(x);
)";

  GarbageCollector gc;

  // CodeObject func;
  FunctionObject * fn = gc.alloc<FunctionObject>( "add", 2 );
  fn->code_object.emit_instr( OP_LOAD_LOCAL, 0 ); // a
  fn->code_object.emit_instr( OP_LOAD_LOCAL, 1 ); // b
  fn->code_object.emit_instr( OP_ADD );
  fn->code_object.emit_instr( OP_RETURN );

  CodeObject code;
  code.names.push_back( "add" );
  code.names.push_back( "x" );
  code.emit_literal( Object::Function( fn ) );
  code.emit_instr( OP_MAKE_FUNCTION );
  code.emit_instr( OP_STORE_GLOBAL, 0 );

  code.emit_literal( Object::Integer( 2 ) );
  code.emit_literal( Object::Integer( 3 ) );
  code.emit_instr( OP_LOAD_GLOBAL, 0 ); // add
  code.emit_instr( OP_CALL_FUNCTION );
  code.emit_instr( OP_STORE_GLOBAL, 1 ); // x

  code.emit_instr( OP_LOAD_GLOBAL, 1 ); // x
  code.emit_instr( OP_DEBUG_PRINT );

  VirtualMachine vm( out, err, gc );

  int r = vm.run( &code );

  ASSERT_EQ( r, 0 );
  ASSERT_EQ( out.str(), "5" );
  ASSERT_EQ( err.str(), "" );
}
#endif

TEST_F( Unittest, test_005 )
{
  const char * src = R"(
print(42);
  )";

  int r = eval( src, out, err );

  ASSERT_EQ( r, 0 );
  ASSERT_EQ( out.str(), "42" );
  ASSERT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_fn_decl )
{
  const char * src = R"(
fn meaning_of_life() {
  return 42;
}

print(meaning_of_life());
  )";

  ( void ) eval( src, out, err );

  ASSERT_EQ( out.str(), "42" );
  ASSERT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_var_decl )
{
  const char * src = R"(
var x = 2 + 3;

print(x);
  )";

  ( void ) eval( src, out, err );

  ASSERT_EQ( out.str(), "5" );
  ASSERT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_block_02 )
{
  const char * src = R"(
fn foo(a, b) {
  var c = a + b;
  return c;
}

print( foo(2, 3) );
  )";

  ( void ) eval( src, out, err );

  ASSERT_EQ( out.str(), "5" );
  ASSERT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_block_01 )
{
  const char * src = R"(
var a = 1;
{
  var b = 2;
  print(b);
}
print(a);
  )";

  ( void ) eval( src, out, err );

  ASSERT_EQ( out.str(), "21" );
  ASSERT_EQ( err.str(), "" );
}


