#pragma once

#include <string>

#include "bytecode.h"
#include "gc.h"

class Object;

struct FunctionObject : public GarbageCollected
{
  char name[32];
  uint8_t num_args = 0;
  //uint16_t num_locals = 0;
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
    OBJECT,
  };

  Object();
  static Object Boolean( bool );
  static Object Integer( int );
  static Object Real( double );
  static Object String( const char * );
  static Object Function( FunctionObject * );

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
    ObjectObject * object;
  };
};