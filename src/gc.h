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

  bool is_marked() const
  {
    return m_marked;
  }

protected:
  bool m_marked;
};

// TODO: implement mark & sweep
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

  void sweep()
  {
    for( auto it = m_heap.begin(); it != m_heap.end(); )
    {
      GarbageCollected * gc_obj = *it;
      if( !gc_obj->is_marked() )
      {
        delete gc_obj;
        it = m_heap.erase( it );
      }
      else
      {
        it++;
      }
    }
  }

private:
  std::list<GarbageCollected *> m_heap;
};
