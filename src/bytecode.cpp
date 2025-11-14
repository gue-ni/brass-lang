#include "bytecode.h"
#include "object.h"

// size of a single instruction in bytes
constexpr size_t INSTR_SIZE = 3;

std::pair<uint8_t, uint8_t> short_to_bytes( uint16_t u16 )
{
  uint16_t hi = ( u16 >> 8 ) & 0xff;
  uint16_t lo = u16 & 0xff;
  return std::make_pair( ( uint8_t ) hi, ( uint8_t ) lo );
}

void CodeObject::emit_instr( OpCode instr )
{
  instructions.push_back( instr );
}

void CodeObject::emit_instr( OpCode instr, uint16_t arg )
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

size_t CodeObject::emit_jump( OpCode op )
{
  size_t jump_start = instructions.size();
  emit_instr( op, 0xffff );
  return jump_start;
}

void CodeObject::end_jump( size_t jump_start )
{
  size_t jump_end = instructions.size();
  size_t jump_len = jump_end - ( jump_start + INSTR_SIZE );

  auto [hi, lo] = short_to_bytes( jump_len );

  instructions[jump_start + 1] = hi;
  instructions[jump_start + 2] = lo;
}

void CodeObject::emit_loop( size_t start )
{
  size_t end    = instructions.size() + INSTR_SIZE;
  size_t offset = end - start;
  emit_instr( OP_LOOP, ( uint16_t ) offset );
}

uint16_t CodeObject::emit_name( const std::string & name )
{
  uint16_t index = ( uint16_t ) names.size();
  names.push_back( name );
  return index;
}

CodeObject * CodeObject::get_root()
{
  CodeObject * current = this;
  if( current->parent != nullptr )
  {
    current = current->parent;
  }
  return current;
}
