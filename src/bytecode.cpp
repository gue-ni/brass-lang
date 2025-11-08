#include "bytecode.h"

void CodeObject::emit_instr( Instruction instr )
{
  instructions.push_back( instr );
}

void CodeObject::emit_literal( Value value )
{
  size_t index = literals.size();
  literals.push_back( value );
  emit_instr( OP_LOAD_CONST );
  instructions.push_back( static_cast<uint8_t>( index ) );
}
