#include <gtest/gtest.h>

#include "brass.h"

TEST( tests, test_01 )
{
  ASSERT_EQ( 2 + 2, 5 );
}

TEST( tests, test_02 )
{
  int r = eval( "" );
  ASSERT_EQ( r, 0 );
}
