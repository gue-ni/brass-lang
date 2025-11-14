#include "builtin.h"
#include "vm.h"
#include <cassert>

Object f_typeof( VirtualMachine * vm, int argc, Object args[] )
{
  assert( argc == 1 );
  Object arg0 = args[0];
  StringObject * str;
  switch( arg0.type )
  {
    case Object ::Type ::NIL :
      str = vm->gc().alloc<StringObject>( "niltype" );
      break;
    case Object ::Type ::INTEGER :
      str = vm->gc().alloc<StringObject>( "integer" );
      break;
    case Object ::Type ::STRING :
      str = vm->gc().alloc<StringObject>( "string" );
      break;
    default :
      str = vm->gc().alloc<StringObject>( "unknown-type" );
      break;
  }

  return Object::String( str );
}
