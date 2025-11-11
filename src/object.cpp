#include "object.h"
#include "utils.h"
#include <cassert>
#include <cstring>

Object::Object()
    : type( NIL )
{
}

Object::Object( const Object & other)
{
  copy_from(other);
}

Object & Object::operator=( const Object & other )
{
  if (this != &other)
  {
    copy_from(other);
  }
  return *this;
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

void Object::copy_from( const Object & other )
{
  type = other.type;
  switch (type) {
    case INTEGER :
      integer = other.integer;
      break;
    case FUNCTION :
      function = other.function;
      break;
      default :
        assert(false);
  }
}

FunctionObject::FunctionObject( const char * fn_name, uint8_t arity, CodeObject* ctx )
    : name( STRDUP( fn_name ) )
    , num_args( arity )
{
  code_object.parent = ctx;
}

ClassObject::ClassObject( const char * cl_name )
    : name( STRDUP( cl_name ) )
{
}

InstanceObject::InstanceObject( ClassObject * klass )
    : klass( klass )
{
}

std::ostream & operator<<( std::ostream & os, const Object & obj )
{
  switch( obj.type )
  {
    case Object::Type::NIL :
      os << "NIL";
      break;
    case Object::Type::BOOLEAN :
      os << ( obj.boolean ? "true" : "false" );
      break;
    case Object::Type::INTEGER :
      os << obj.integer;
      break;
    case Object::Type::REAL :
      os << obj.real;
      break;
    case Object::Type::STRING :
      os << obj.string;
      break;
    case Object::Type::FUNCTION :
      os << "function<" << obj.function->name << ">";
      break;
    case Object::Type::CLASS :
      os << "class<" << obj.klass->name << ">";
      break;
    case Object::Type::INSTANCE :
      os << "instance<" << obj.instance->klass->name << ">";
      break;
    default :
      assert( false );
      break;
  }
  return os;
}


