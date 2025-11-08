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
  ASSERT_EQ( r, 1 );
  ASSERT_EQ( out.str(), "" );
  ASSERT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_001 )
{
  GarbageCollector gc;
  VirtualMachine vm( out, err, gc );

  CodeObject code;

  Value a( 2 );
  Value b( 3 );

  code.instructions.push_back( OP_LOAD_CONST );
  code.instructions.push_back( 0 );
  code.instructions.push_back( OP_LOAD_CONST );
  code.instructions.push_back( 1 );
  code.instructions.push_back( OP_BINARY_ADD );
  code.instructions.push_back( OP_DEBUG_PRINT );
  code.literals.push_back( a );
  code.literals.push_back( b );

  int r = vm.run( &code );
  ASSERT_EQ( r, 0 );
  ASSERT_EQ( out.str(), "" );
  ASSERT_EQ( err.str(), "" );
}
