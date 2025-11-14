#pragma once

#include <ostream>
#include <string>

#include "bytecode.h"
#include "gc.h"
#include "utils.h"

class Object;

class VirtualMachine;

typedef Object ( *NativeFunction )( VirtualMachine *, int, Object[] );

struct StringObject : public GarbageCollected
{
  char * str;
  StringObject( const char * s );
  ~StringObject();
};

struct FunctionObject : public GarbageCollected
{
  char * name;
  uint8_t num_args = 0;
  CodeObject code_object;
  FunctionObject( const char * fn_name, uint8_t arity, CodeObject * ctx );
  ~FunctionObject()
  {
    if( name )
    {
      free( name );
      name = nullptr;
    }
  }
};

struct ListObject
    : public GarbageCollected
    , public LinkedList<Object>
{
};

struct MapObject
    : public GarbageCollected
    , public HashMap<Object>
{
};

struct ClassObject : public GarbageCollected
{
  char * name;
  ClassObject( const char * cl_name );
  ~ClassObject();
};

struct InstanceObject : public GarbageCollected
{
  ClassObject * klass;
  HashMap<Object> fields;
  InstanceObject( ClassObject * klass );
  ~InstanceObject();
};

class Object
{
public:
  enum Type
  {
    NIL,
    BOOLEAN,
    INTEGER,
    REAL,
    STRING,
    LIST,
    MAP,
    FUNCTION,
    NATIVE,
    CLASS,
    INSTANCE,
  };

  Object();

  static Object Nil();
  static Object Boolean( bool );
  static Object Integer( int );
  static Object Real( double );
  static Object String( StringObject * );
  static Object Function( FunctionObject * );
  static Object Native( NativeFunction );
  static Object Class( ClassObject * );
  static Object Instance( InstanceObject * );

  Type type;
  union
  {
    bool boolean;
    int integer;
    double real;
    StringObject * string;
    ListObject * list;
    MapObject * map;
    FunctionObject * function;
    NativeFunction native;
    ClassObject * klass;
    InstanceObject * instance;
  };

  bool is_falsy() const;
  bool is_truthy() const;
};

std::ostream & operator<<( std::ostream &, const Object & );
