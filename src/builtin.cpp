#include "builtin.h"
#include <cassert>

Object f_typeof( int argc, Object args[] )
{
  assert( argc == 1 );
  Object arg0 = args[0];
  switch( arg0.type )
  {
    case Object ::Type ::NIL :
      return Object::String( "nil-type" );
    case Object ::Type ::INTEGER :
      return Object::String( "integer" );
    case Object ::Type ::STRING :
      return Object::String( "string" );
    default :
      return Object::String( "unknown type" );
  }
}
