#include "object.h"
#include "utils.h"
#include <cstring>

Object::Object()
    : type( NIL )
{
}

Object Object::Nil()
{
  return Object();
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

Object Object::Function( FunctionObject * fn )
{
  Object obj;
  obj.type     = FUNCTION;
  obj.function = fn;
  return obj;
}

Object Object::Class( ClassObject * c )
{
  Object obj;
  obj.type  = CLASS;
  obj.klass = c;
  return obj;
}

Object Object::Instance( InstanceObject * i )
{
  Object obj;
  obj.type     = INSTANCE;
  obj.instance = i;
  return obj;
}

FunctionObject::FunctionObject( const char * fn_name, uint8_t arity )
    : name( STRDUP( fn_name ) )
    , num_args( arity )
{
}

ClassObject::ClassObject( const char * cl_name )
    : name( STRDUP( cl_name ) )
{
}

InstanceObject::InstanceObject( ClassObject * klass )
    : klass( klass )
{
}
