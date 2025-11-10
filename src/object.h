#pragma once

#include <ostream>
#include <string>

#include "bytecode.h"
#include "gc.h"

class Object;

struct FunctionObject : public GarbageCollected
{
  char * name;
  uint8_t num_args = 0;
  CodeObject code_object;
  FunctionObject( const char * fn_name, uint8_t arity );
};

struct ListObject : public GarbageCollected
{
  Object * items;
  size_t capacity;
  size_t length;
};

struct MapObject : public GarbageCollected
{
};

struct ObjectObject : public GarbageCollected
{
  // TODO: add fields and a type
};

struct ClassObject : public GarbageCollected
{
  char * name;
  ClassObject( const char * cl_name );
};

struct InstanceObject : public GarbageCollected
{
  ClassObject * klass;
  InstanceObject( ClassObject * klass );
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
    CLASS,
    INSTANCE,
  };

  Object();
  static Object Nil();
  static Object Boolean( bool );
  static Object Integer( int );
  static Object Real( double );
  static Object String( const char * );
  static Object Function( FunctionObject * );
  static Object Class( ClassObject * );
  static Object Instance( InstanceObject * );

  Type type;
  union
  {
    bool boolean;
    int integer;
    double real;
    char * string;
    ListObject * list;
    MapObject * map;
    FunctionObject * function;
    ClassObject * klass;
    InstanceObject * instance;
  };
};

std::ostream & operator<<( std::ostream &, const Object & );
