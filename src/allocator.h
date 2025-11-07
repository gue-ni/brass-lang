#pragma once

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <list>
#include <memory_resource>
#include <new>
#include <utility>

// https://nullprogram.com/blog/2023/09/27/
// https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator
class ArenaAllocator
{
public:
  ArenaAllocator( const ArenaAllocator & )             = delete;
  ArenaAllocator & operator=( const ArenaAllocator & ) = delete;

  ArenaAllocator( std::size_t size )
      : m_size( size )
      , m_offset( 0 )
  {
    // printf( "%s size=%lu\n", __FUNCTION__, size );
    m_base = ( uint8_t * ) malloc( size );
    if( !m_base )
    {
      throw std::bad_alloc();
    }
  }

  ~ArenaAllocator()
  {
    // printf( "%s used=%lu\n", __FUNCTION__, m_offset );
    if( m_base )
    {
      free( m_base );
      m_base = nullptr;
    }
  }

  template <typename T, typename... Args>
  T * alloc( Args &&... args )
  {
    void * memory = alloc( sizeof( T ), alignof( T ) );
    return new( memory ) T( std::forward<Args>( args )... );
  }

private:
  std::size_t m_size;
  std::size_t m_offset;
  uint8_t * m_base;

  // Get pointer at the offset, advance the offset by the size of the object, and return the pointer.
  void * alloc( std::size_t size, std::size_t alignment )
  {
    std::size_t base              = reinterpret_cast<std::size_t>( m_base );
    std::size_t current           = base + m_offset;
    std::size_t alignment_padding = ( alignment - ( current % alignment ) ) % alignment;
    std::size_t new_offset        = m_offset + alignment_padding + size;

    if( m_size < new_offset )
    {
      throw std::bad_alloc();
    }

    std::size_t aligned = current + alignment_padding;

    // printf( "%s size=%lu, alignment=%lu\n", __PRETTY_FUNCTION__, size, alignment );
    // printf( "%s base=%lu current=%lu, offset=%lu\n", __PRETTY_FUNCTION__, base, current, m_offset );
    // printf( "%s padding=%ld\n", __PRETTY_FUNCTION__, alignment_padding );
    // printf( "%s new_offset=%ld, memory=%ld\n", __PRETTY_FUNCTION__, new_offset, aligned );

    m_offset = new_offset;

    return reinterpret_cast<void *>( aligned );
  }
};