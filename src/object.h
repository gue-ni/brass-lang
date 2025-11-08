#pragma once

#include <string>

#include "gc.h"

struct CodeObject;


class Object : public GarbageCollected
{
};

struct Function : public GarbageCollected
{
  char name[32];
  uint8_t arity = 0;
  CodeObject * code_object;
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