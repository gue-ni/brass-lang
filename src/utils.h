#pragma once

#include <cstring>

using uint = unsigned int;

#ifdef WIN32
#define STRDUP(x) _strdup(x)
#else
#define STRDUP(x) strdup(x)
#endif

template <typename T>
class HashMap
{
public:
  HashMap( size_t capacity = 256 )
  {
    m_capacity = capacity;
    m_entries  = new Entry *[m_capacity];
    for( size_t i = 0; i < m_capacity; i++ )
    {
      m_entries[i] = nullptr;
    }
  }

  ~HashMap()
  {
  }

  void set( const char * key, const T & value )
  {
    size_t idx = hash( key );

#if 0
    Entry * curr = m_entries[idx];
    while( curr )
    {
      curr = curr->next;
    }
#endif

    m_entries[idx] = new Entry( key, value, m_entries[idx] );
  }

  bool contains( const char * key )
  {
    return false;
  }

  T get( const char * key )
  {
    size_t idx = hash( key );
    return T();
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
  Entry ** m_entries;

  size_t hash( const char * str ) const
  {
    size_t hash = 5381;
    int c;
    while( ( c = *str++ ) )
      hash = ( ( hash << 5 ) + hash ) + c; // hash * 33 + c
    return hash % m_capacity;
  }
};
