#pragma once

#include <string>

#include "gc.h"

struct CodeObject;

class Object;

struct FunctionObject : public GarbageCollected
{
  char name[32];
  uint8_t arity = 0;
  CodeObject * code_object;
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
  };

  Object();
  static Object Boolean( bool );
  static Object Integer( int );
  static Object Real( double );
  static Object String( const char * );

  Type type;
  union
  {
    bool boolean;
    int integer;
    double real;
    char * string;
    ListObject * list;
    MapObject * map;
    FunctionObject* function;
  };
};