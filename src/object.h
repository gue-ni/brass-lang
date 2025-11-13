#pragma once

#include <ostream>
#include <string>

#include "bytecode.h"
#include "gc.h"
#include "utils.h"

class Object;

struct FunctionObject : public GarbageCollected
{
  char * name;
  uint8_t num_args = 0;
  CodeObject code_object;
  FunctionObject( const char * fn_name, uint8_t arity, CodeObject* ctx );
  ~FunctionObject()
  {
    if (name)
    {
      free(name);
      name = nullptr;
    }
  }
};

struct ListObject : public GarbageCollected
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
  // HashMap<Object> methods;
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

  bool is_falsy() const;
  bool is_truthy() const;
};

std::ostream & operator<<( std::ostream &, const Object & );
