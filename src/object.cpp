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

Object Object::Integer( int value )
{
  Object obj;
  obj.type    = INTEGER;
  obj.integer = value;
  return obj;
}

Object Object::Real( double value )
{
  Object obj;
  obj.type = REAL;
  obj.real = value;
  return obj;
}

Object Object::String( const char * value )
{
  Object obj;
  obj.type   = STRING;
  obj.string = STRDUP( value );
  return obj;
}
