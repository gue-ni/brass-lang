#include "object.h"
#include "utils.h"
#include <cassert>
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

Object Object::Native( NativeFunction fn )
{
  Object obj;
  obj.type   = NATIVE;
  obj.native = fn;
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

bool Object::is_falsy() const
{
  return !is_truthy();
}

bool Object::is_truthy() const
{
  switch( type )
  {
    case Object::NIL :
      return false;
    case Object::BOOLEAN :
      return boolean;
    case Object::INTEGER :
      return integer != 0;
    case Object::REAL :
      return real != 0;
    case Object::STRING :
      return string != nullptr && 0 < strlen( string );
    case Object::LIST :
    case Object::MAP :
    case Object::FUNCTION :
    case Object::CLASS :
    case Object::INSTANCE :
    default :
      return true;
  }
}

FunctionObject::FunctionObject( const char * fn_name, uint8_t arity, CodeObject * ctx )
    : name( STRDUP( fn_name ) )
    , num_args( arity )
{
  code_object.parent = ctx;
}

ClassObject::ClassObject( const char * cl_name )
    : name( STRDUP( cl_name ) )
{
}

ClassObject::~ClassObject()
{
  if( name )
  {
    free( name );
    name = nullptr;
  }
}

InstanceObject::InstanceObject( ClassObject * klass )
    : klass( klass )
{
}

InstanceObject::~InstanceObject()
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
