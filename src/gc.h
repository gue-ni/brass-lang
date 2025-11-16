#pragma once

#include <list>

class GarbageCollected
{
public:
  GarbageCollected()
      : m_marked( false )
  {
  }

  virtual ~GarbageCollected()
  {
  }

  virtual void mark()
  {
    m_marked = true;
  }

protected:
  bool m_marked;
};

class GarbageCollector
{
public:
  template <typename T, typename... Args>
  T * alloc( Args &&... args )
  {
    T * object = new T( std::forward<Args>( args )... );
    m_heap.push_back( object );
    return object;
  }

  GarbageCollector()
  {
  }

  ~GarbageCollector()
  {
    for( GarbageCollected * gc_obj : m_heap )
    {
      delete gc_obj;
    }
  }

private:
  std::list<GarbageCollected *> m_heap;
};
