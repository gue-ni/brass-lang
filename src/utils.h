#pragma once

#include <cstring>
#include <string>

using uint = unsigned int;

#ifdef WIN32
#define STRDUP( x ) _strdup( x )
#else
#define STRDUP( x ) strdup( x )
#endif

template <typename NodeType>
struct Result
{
  std::string error;
  NodeType * node;
  bool ok() const
  {
    return error.empty() && node != nullptr;
  }
};

template <typename T>
Result<T> make_result( T * node )
{
  Result<T> r;
  r.node = node;
  return r;
}

template <typename T>
Result<T> make_error( const std::string & error )
{
  Result<T> r;
  r.error = error;
  return r;
}

template <typename T>
class LinkedList
{
public:
  struct Node
  {
    T value;
    Node * next;
    Node( const T & v, Node * n )
        : value( v )
        , next( n )
    {
    }
  };

  LinkedList()
      : m_head( nullptr )
      , m_tail( nullptr )
  {
  }

  ~LinkedList()
  {
    Node * curr = m_head;
    while( curr )
    {
      Node * tmp = curr;
      curr       = curr->next;
      delete tmp;
    }
    m_head = nullptr;
    m_tail = nullptr;
  }

  void push_back( const T & value )
  {
    push_back( new Node( value, nullptr ) );
  }

  void push_back( Node * node )
  {
    if( !m_head )
      m_head = node;

    if( m_tail )
      m_tail->next = node;

    m_tail = node;
  }

  Node * head()
  {
    return m_head;
  }

  Node * tail()
  {
    return m_tail;
  }

private:
  Node * m_head;
  Node * m_tail;
};

template <typename T>
class HashMap
{
public:
  HashMap( size_t capacity = 256 )
  {
    m_capacity = capacity;
    m_table    = new Entry *[m_capacity];
    for( size_t i = 0; i < m_capacity; i++ )
    {
      m_table[i] = nullptr;
    }
  }

  ~HashMap()
  {
    clear();
    delete[] m_table;
  }

  void set( const char * key, const T & value )
  {
    size_t idx   = hash( key );
    Entry * curr = m_table[idx];
    while( curr )
    {
      if( strcmp( curr->key, key ) == 0 )
      {
        curr->value = value;
        return;
      }
      curr = curr->next;
    }
    m_table[idx] = new Entry( key, value, m_table[idx] );
  }

  bool get( const char * key, T & value )
  {
    size_t idx   = hash( key );
    Entry * curr = m_table[idx];
    while( curr )
    {
      if( strcmp( curr->key, key ) == 0 )
      {
        value = curr->value;
        return true;
      }
      curr = curr->next;
    }
    return false;
  }

private:
  struct Entry
  {
    char * key;
    T value;
    Entry * next;
    Entry( const char * k, T v, Entry * nxt )
        : key( STRDUP( k ) )
        , value( v )
        , next( nxt )
    {
    }
  };

  size_t m_capacity;
  Entry ** m_table;

  size_t hash( const char * str ) const
  {
    size_t hash = 5381;
    int c;
    while( ( c = *str++ ) )
      hash = ( ( hash << 5 ) + hash ) + c; // hash * 33 + c
    return hash % m_capacity;
  }

  void clear()
  {
    for( size_t i = 0; i < m_capacity; i++ )
    {
      Entry * curr = m_table[i];
      while( curr )
      {
        Entry * tmp = curr;
        curr        = curr->next;
        free( tmp->key );
        delete tmp;
      }
      m_table[i] = nullptr;
    }
  }
};
