#include "object.h"
#include "utils.h"

Object::Object()
    : type( NIL )
{
}

Object Object::Boolean( bool value )
{
  Object obj;
  obj.type    = BOOLEAN;
  obj.boolean = value;
  return obj;
}

Object Object::Integer( int i )
{
  Object obj;
  obj.type    = INTEGER;
  obj.integer = i;
  return obj;
}

Object Object::String( const char * str )
{
  Object obj;
  obj.type   = STRING;
  obj.string = STRDUP( str );
  return obj;
}
