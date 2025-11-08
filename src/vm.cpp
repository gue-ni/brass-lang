#include "vm.h"
#include "object.h"

VirtualMachine::VirtualMachine( std::ostream & out, std::ostream & err, GarbageCollector & gc )
    : m_out( out )
    , m_err( err )
    , m_gc( gc )
{
}

int VirtualMachine::run( CodeObject * co )
{
  m_frames.push( Frame( co ) );

  while( !m_exit && current_frame().ip != current_frame().code_object->instructions.end() )
  {
    auto [instr, arg] = next_instr();

    switch( instr )
    {
      case OP_LOAD_CONST :
        {
          Value constant = current_frame().code_object->literals[arg];
          push( constant );
          break;
        }
      case OP_BINARY_ADD :
        {
          Value lhs    = pop();
          Value rhs    = pop();
          int a        = lhs.integer;
          int b        = rhs.integer;
          Value result = Value( a + b );
          push( result );
          break;
        }
      case OP_DEBUG_PRINT :
        {
          Value obj = pop();
          m_out << obj.integer;
          break;
        }
      default :
        m_exit = true;
        break;
    }
  }
  return 0;
}

void VirtualMachine::push( Value obj )
{
  current_frame().stack.push( obj );
}

Value VirtualMachine::pop()
{
  Value obj = current_frame().stack.top();
  current_frame().stack.pop();
  return obj;
}

Frame & VirtualMachine::current_frame()
{
  return m_frames.top();
}

std::pair<Instruction, uint16_t> VirtualMachine::next_instr()
{
  Instruction instr = static_cast<Instruction>( *( current_frame().ip++ ) );
  switch( instr )
  {
    case OP_LOAD_CONST :
    case OP_LOAD_VAR :
    case OP_STORE_VAR :
      {
        uint8_t arg = *( current_frame().ip++ );
        return std::make_pair( instr, ( uint16_t ) arg );
      }
    case OP_DEBUG_PRINT :
    case OP_BINARY_ADD :
    default :
      return std::make_pair( instr, 0xffff );
  }
}
