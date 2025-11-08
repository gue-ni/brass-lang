#include <gtest/gtest.h>

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
  GarbageCollector gc;
  VirtualMachine vm( out, err, gc );

  CodeObject code;
  code.emit_literal( Value( 2 ) );
  code.emit_literal( Value( 3 ) );
  code.emit_instr( OP_BINARY_ADD );
  code.emit_instr( OP_DEBUG_PRINT );

  int r = vm.run( &code );
  ASSERT_EQ( r, 0 );
  ASSERT_EQ( out.str(), "5" );
  ASSERT_EQ( err.str(), "" );
}
