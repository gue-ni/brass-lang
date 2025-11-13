#include "vm.h"
#include "object.h"
#include <cassert>
#include <iomanip>

#define RUNTIME_ERROR( msg )       \
  do                               \
  {                                \
    m_runtime_error_message = msg; \
    goto label_runtime_error;      \
  } while( 0 )

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
    auto [op, arg] = next_instr();
    switch( op )
    {
      case OP_LOAD_CONST :
        {
          Object obj = current_frame().code_object->literals[arg];
          push( obj );
          break;
        }
      case OP_LOAD_GLOBAL :
        {
          CodeObject * current = current_frame().code_object;
          CodeObject * global  = current->get_root(); // something is not right here

          assert( global != nullptr );
          assert( arg < global->names.size() );

          std::string var = global->names[arg]; // this throws an error
          auto it         = m_globals.find( var );
          if( it != m_globals.end() )
          {
            push( it->second );
          }
          else
          {
            push( Object::Nil() );
          }
          break;
        }
      case OP_STORE_GLOBAL :
        {
          CodeObject * current = current_frame().code_object;
          CodeObject * global  = current->get_root();

          assert( global != nullptr );
          assert( arg < global->names.size() );

          std::string var = global->names[arg];
          Object obj      = pop(); // when we get the function with pop it is corrupted
          m_globals[var]  = obj;
          break;
        }
      case OP_LOAD_LOCAL :
        {
          size_t slot = current_frame().bp + arg;
          assert( slot < m_stack.size() );
          Object obj = m_stack[slot];
          push( obj );
          break;
        }
      case OP_STORE_LOCAL :
        {
          Object obj  = pop();
          size_t slot = current_frame().bp + arg;
          if( !( slot < m_stack.size() ) )
          {
            RUNTIME_ERROR( "Variable not declard" );
          }
          m_stack[slot] = obj;
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
      case OP_MULT :
        {
          Object lhs    = pop();
          Object rhs    = pop();
          Object result = Object::Integer( lhs.integer * rhs.integer );
          push( result );
          break;
          break;
        }
      case OP_DIV :
        {
          Object lhs = pop();
          Object rhs = pop();
          if( rhs.integer == 0 )
          {
            RUNTIME_ERROR( "Division by zero" );
          }
          Object result = Object::Integer( lhs.integer / rhs.integer );
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
            RUNTIME_ERROR( "Error: not a callable object" );
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
      case OP_GET_PROPERTY :
        {
          CodeObject * global = global_code_object();
          std::string name    = global->names[arg];
          Object obj          = pop();

          if( obj.type == Object::Type::INSTANCE )
          {
            Object property = Object::Nil();
            obj.instance->fields.get( name.c_str(), property );
            push( property );
          }
          else
          {
            RUNTIME_ERROR( "not a object" );
          }
          break;
        }
      case OP_SET_PROPERTY :
        {
          CodeObject * global = global_code_object();
          std::string name    = global->names[arg];

          Object obj      = pop();
          Object property = pop();
          if( obj.type == Object::Type::INSTANCE )
          {
            obj.instance->fields.set( name.c_str(), property );
          }
          else
          {
            RUNTIME_ERROR( "asdf" );
          }
          break;
        }
      case OP_JMP :
        {
          current_frame().ip += arg;
          break;
        }
      case OP_JMP_IF_FALSE :
        {
          Object obj = pop();
          if( obj.is_falsy() )
          {
            current_frame().ip += arg;
          }
          break;
        }
      case OP_LOOP :
        {
          current_frame().ip -= arg;
          break;
        }
      default :
        m_err << "Unhandled instruction: 0x" << std::hex << std::setw( 2 ) << std::setfill( '0' )
              << static_cast<int>( op ) << "\n";
        m_exit = true;
        goto label_runtime_error;
        break;
    }
  }
  return 0;

label_runtime_error:
  m_err << "RUNTIME ERROR: " << m_runtime_error_message << std::endl;
  return 1;
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

CodeObject * VirtualMachine::current_code_object()
{
  return current_frame().code_object;
}

CodeObject * VirtualMachine::global_code_object()
{
  return current_code_object()->get_root();
}

std::pair<OpCode, uint16_t> VirtualMachine::next_instr()
{
  OpCode op = static_cast<OpCode>( *( current_frame().ip++ ) );
  switch( op )
  {
    case OP_LOAD_CONST :
    case OP_LOAD_GLOBAL :
    case OP_STORE_GLOBAL :
    case OP_LOAD_LOCAL :
    case OP_STORE_LOCAL :
    case OP_GET_PROPERTY :
    case OP_SET_PROPERTY :
    case OP_JMP :
    case OP_JMP_IF_FALSE :
    case OP_LOOP :
      {
        uint8_t hi   = *( current_frame().ip++ );
        uint8_t lo   = *( current_frame().ip++ );
        uint16_t arg = ( uint16_t( hi ) << 8 ) | ( uint16_t( lo ) );
        return std::make_pair( op, arg );
      }
    default :
      return std::make_pair( op, 0xffff );
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
