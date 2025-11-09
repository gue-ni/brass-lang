#include "vm.h"
#include "object.h"
#include <cassert>

VirtualMachine::VirtualMachine( std::ostream & out, std::ostream & err, GarbageCollector & gc )
    : m_out( out )
    , m_err( err )
    , m_gc( gc )
{
}

int VirtualMachine::run( CodeObject * co )
{
  m_frames.push( Frame( co ) );

  while( !m_exit && ( current_frame().ip != current_frame().code_object->instructions.end() ) )
  {
    auto [instr, arg] = next_instr();
    switch( instr )
    {
      case OP_LOAD_CONST :
        {
          Object obj = current_frame().code_object->literals[arg];
          push( obj );
          break;
        }
      case OP_LOAD_GLOBAL :
        {
          std::string var = current_frame().code_object->names[arg];
          auto it         = m_globals.find( var );
          if( it != m_globals.end() )
          {
            push( it->second );
          }
          break;
        }
      case OP_STORE_GLOBAL :
        {
          std::string var = current_frame().code_object->names[arg];
          Object obj      = pop();
          m_globals[var]  = obj;
          break;
        }
      case OP_LOAD_LOCAL :
        {
          Object obj = m_stack[current_frame().bp + arg];
          push( obj );
          break;
        }
      case OP_STORE_LOCAL : {
          Object obj = pop();
          m_stack[current_frame().bp + arg] = obj;
          break;
        }
      case OP_ADD :
        {
          Object lhs    = pop();
          Object rhs    = pop();
          int a         = lhs.integer;
          int b         = rhs.integer;
          Object result = Object::Integer( a + b );
          push( result );
          break;
        }
      case OP_DEBUG_PRINT :
        {
          Object obj = pop();
          m_out << obj.integer;
          break;
        }
      case OP_MAKE_FUNCTION :
        {
          // TODO: setup environment
          break;
        }
      case OP_CALL_FUNCTION :
        {
          Object obj = pop();
          assert( obj.type == Object::Type::FUNCTION );
          FunctionObject * fn = obj.function;
          size_t stack_size = m_stack.size();
          size_t new_stack_size = stack_size + (fn->num_locals - fn->num_args);
          size_t bp           = m_stack.size() - fn->num_args;
          //m_stack.resize(m_stack.size() + (fn->num_locals - fn->num_args));
          m_stack.resize(new_stack_size);
          m_frames.push( Frame( &fn->code_object, bp ) );
          break;
        }
      case OP_RETURN :
        {
          Object return_value = pop();
          Frame & frame       = m_frames.top();
          m_stack.resize( frame.bp );
          m_frames.pop();
          push( return_value );
          break;
        }
      default :
        m_exit = true;
        break;
    }
  }
  return 0;
}

void VirtualMachine::push( Object obj )
{
  m_stack.push_back( obj );
}

Object VirtualMachine::pop()
{
  Object obj = m_stack.back();
  m_stack.pop_back();
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
    case OP_LOAD_GLOBAL :
    case OP_STORE_GLOBAL :
    case OP_LOAD_LOCAL :
    case OP_STORE_LOCAL :
      {
        uint8_t hi   = *( current_frame().ip++ );
        uint8_t lo   = *( current_frame().ip++ );
        uint16_t arg = ( uint16_t( hi ) << 8 ) | ( uint16_t( lo ) );
        return std::make_pair( instr, arg );
      }
    default :
      return std::make_pair( instr, 0xffff );
  }
}
