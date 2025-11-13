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
  const char * src = "print 2 + 3;";
  int r            = eval( src, out, err );
  EXPECT_EQ( r, 0 );
  EXPECT_EQ( out.str(), "5" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_001 )
{
  const char * src = "print 1 + 2 + 3;";
  int r            = eval( src, out, err );
  EXPECT_EQ( r, 0 );
  EXPECT_EQ( out.str(), "6" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_002 )
{
  const char * src = "print 1 + 2 * 3 + 4;";
  int r            = eval( src, out, err );
  EXPECT_EQ( r, 0 );
  EXPECT_EQ( out.str(), "11" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_005 )
{
  const char * src = "print 45 - 3;";
  int r            = eval( src, out, err );
  EXPECT_EQ( r, 0 );
  EXPECT_EQ( out.str(), "42" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_fn_03 )
{
  const char * src = R"(
fn meaning_of_life() {
  return 42;
}

print meaning_of_life();
  )";

  ( void ) eval( src, out, err );

  EXPECT_EQ( out.str(), "42" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_var_decl_01 )
{
  const char * src = R"(
var x = 2 + 3;
print x;
  )";

  ( void ) eval( src, out, err );

  EXPECT_EQ( out.str(), "5" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_fn_00 )
{
  const char * src = R"(
fn foo(a, b) {
  var c = a + b;
  return c;
}

print foo(2, 3);
  )";

  ( void ) eval( src, out, err );

  EXPECT_EQ( out.str(), "5" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_fn_01 )
{
  const char * src = R"(
var d = 5;

fn foo(a) {
  return a + d;
}

print foo(2);
  )";

  ( void ) eval( src, out, err );

  EXPECT_EQ( out.str(), "7" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_fn_02 )
{
  const char * src = R"(
var d = 5;

fn foo(a, b) {
  var c = a + b;
  return c + d;
}

print foo(2, 3);
  )";

  ( void ) eval( src, out, err );

  EXPECT_EQ( out.str(), "10" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_block_01 )
{
  const char * src = R"(
var a = 1;
print a;
{
  var b = 2;
  print b;
}
  )";

  ( void ) eval( src, out, err );

  EXPECT_EQ( out.str(), "12" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_block_03 )
{
  const char * src = R"(
var a = 1;
print a;
{
  var b = 2;
  print b;
  {
    var c = b + 1;
    print c;

    a = b + c;

    b = b + 2;
    print b;
  }
}
print a;
  )";

  ( void ) eval( src, out, err );

  EXPECT_EQ( out.str(), "12345" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_binary_01 )
{
  const char * src = R"(
print 3 - 5;
  )";

  ( void ) eval( src, out, err );

  EXPECT_EQ( out.str(), "-2" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_class_01 )
{
  const char * src = R"(
class Foo {}

println Foo;

var f = Foo();

println f;


  )";

  ( void ) eval( src, out, err );

  EXPECT_EQ( out.str(), "class<Foo>\ninstance<Foo>\n" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_assignment_01 )
{
  const char * src = R"(
var x = 2;

print x;

x = x + 1;

print x;
  )";

  ( void ) eval( src, out, err );

  EXPECT_EQ( out.str(), "23" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_class_02 )
{
  const char * src = R"(
class Foo {}

var foo = Foo();

print foo.bar;
  )";

  ( void ) eval( src, out, err );

  EXPECT_EQ( out.str(), "NIL" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_class_03 )
{
  const char * src = R"(
class Foo {}

var foo = Foo();

foo.bar = 5;

print foo.bar;
  )";

  ( void ) eval( src, out, err );

  EXPECT_EQ( out.str(), "5" );
  EXPECT_EQ( err.str(), "" );
}

TEST_F( Unittest, test_class_04 )
{
  const char * src = R"(
class Square {}

var sq = Square();

sq.w = 2;
sq.h = 3;

fn area(s) {
  return s.w * s.h;
}

print area(sq);
  )";

  ( void ) eval( src, out, err );

  EXPECT_EQ( out.str(), "6" );
  EXPECT_EQ( err.str(), "" );
}
