#include <gtest/gtest.h>

#include "object.h"
#include "utils.h"

TEST( misc, test_map_01 )
{
  HashMap<Object> map;

  map.set( "foo", Object::Integer( 5 ) );
  // EXPECT_EQ( map.contains( "foo" ), true );
  // EXPECT_EQ( map.contains( "bar" ), false );

  map.set( "bar", Object::String( "hello, world" ) );
  // EXPECT_EQ( map.contains( "bar" ), true );
}
