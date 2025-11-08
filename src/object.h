#pragma once

#include "gc.h"

class Object : public GarbageCollected
{
};

class Value
{
public:
  enum Type
  {
    NIL,
    BOOLEAN,
    INTEGER,
    STRING,
    OBJECT
  };

  Value( int value );

  Type type;
  union
  {
    bool boolean;
    int integer;
    char * string;
    Object * object;
  };
};