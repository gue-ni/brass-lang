#include "bytecode.h"
#include "object.h"

void CodeObject::emit_instr( Instruction instr )
{
  instructions.push_back( instr );
}

void CodeObject::emit_instr( Instruction instr, uint16_t arg )
{
  uint16_t hi = ( arg >> 8 ) & 0xff;
  uint16_t lo = arg & 0xff;
  instructions.push_back( instr );
  instructions.push_back( ( uint8_t ) hi );
  instructions.push_back( ( uint8_t ) lo );
}

void CodeObject::emit_literal( Object value )
{
  uint16_t index = ( uint16_t ) literals.size();
  literals.push_back( value );
  emit_instr( OP_LOAD_CONST, index );
}

uint16_t CodeObject::emit_name( const std::string & name )
{
  uint16_t index = ( uint16_t ) names.size();
  names.push_back( name );
  return index;
}

CodeObject * CodeObject::get_root()
{
  CodeObject* current = this;
  if (current->parent != nullptr)
  {
    current = current->parent;
  }
  return current;
}
