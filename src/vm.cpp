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
  m_stack.resize( co->num_locals );

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
      case OP_STORE_LOCAL :
        {
          Object obj                        = pop();
          m_stack[current_frame().bp + arg] = obj;
          break;
        }
      case OP_ADD :
        {
          Object lhs    = pop();
          Object rhs    = pop();
          Object result = Object::Integer( lhs.integer + rhs.integer );
          push( result );
          break;
        }
      case OP_SUB :
        {
          Object lhs    = pop();
          Object rhs    = pop();
          Object result = Object::Integer( lhs.integer - rhs.integer );
          push( result );
          break;
          break;
        }
      case OP_PRINT :
        {
          Object obj = pop();
          m_out << obj;
          break;
        }
      case OP_PRINTLN :
        {
          Object obj = pop();
          m_out << obj << std::endl;
          break;
        }
      case OP_CALL :
        {
          Object obj = pop();
          if( obj.type == Object::Type::FUNCTION )
          {
            call_fn( obj.function );
          }
          else if( obj.type == Object::Type::CLASS )
          {
            call_ctor( obj.klass );
          }
          else
          {
            m_err << "Error: not a callable object" << std::endl;
          }
          break;
        }
      case OP_RETURN :
        {
          Object obj    = pop();
          Frame & frame = m_frames.top();
          m_stack.resize( frame.bp );
          m_frames.pop();
          push( obj );
          break;
        }
      default :
        assert( false );
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

void VirtualMachine::call_fn( FunctionObject * fn )
{
  size_t stack_size     = m_stack.size();
  size_t new_stack_size = stack_size + ( fn->code_object.num_locals - fn->num_args );
  size_t bp             = stack_size - fn->num_args;
  m_stack.resize( new_stack_size );
  m_frames.push( Frame( &fn->code_object, bp ) );
}

void VirtualMachine::call_ctor( ClassObject * cls )
{
  InstanceObject * instance = m_gc.alloc<InstanceObject>( cls );
  push( Object::Instance( instance ) );
}
