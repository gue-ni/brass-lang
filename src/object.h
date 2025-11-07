#pragma once

#include "gc.h"

class Object : public GarbageCollected
{
public:
  enum Type
  {
    NIL,
    BOOLEAN,
    INTEGER,
    STRING,
  };

  Object( int value );

  Type type;
  union
  {
    bool boolean;
    int integer;
    char * string;
  };
};