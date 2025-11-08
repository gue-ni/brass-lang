#include "bytecode.h"

void CodeObject::emit_instr( Instruction instr )
{
  instructions.push_back( instr );
}

void CodeObject::emit_instr( Instruction instr, uint8_t arg )
{
  instructions.push_back( instr );
  instructions.push_back( arg );
}

void CodeObject::emit_literal( Object value )
{
  size_t index = literals.size();
  literals.push_back( value );
  emit_instr( OP_LOAD_CONST, static_cast<uint8_t>( index ) );
}

uint8_t CodeObject::emit_name( const std::string & name )
{
  size_t index = literals.size();
  names.push_back(name);
  return static_cast<uint8_t>(index);
}
