#pragma once

/**
 * @file
 *   This file defines RingBuffer class for framework
 */

#include <cstdint>
#include <optional>
#include <type_traits>
#include <iterator>
#include <cstddef>


namespace mpp {

template< class T >
class ring_iterator {
  static_assert(std::is_same_v<T, std::decay_t<T>>);

public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type   = std::ptrdiff_t;
  using value_type        = typename T::value_type;
  using pointer           = value_type*;
  using reference         = value_type&;
  using parent            = T;

  ring_iterator(parent* aParent, pointer aPtr)
    : m_parent(aParent)
    , m_ptr(aPtr) {}

  ring_iterator(parent& aParent, pointer aPtr)
    : m_parent(&aParent)
    , m_ptr(aPtr) {}

  bool is_head() { return m_ptr == m_parent->m_head; }
  bool is_tail() { return m_ptr == m_parent->m_tail; }

  reference operator*() const { return *m_ptr; }
  pointer operator->() { return m_ptr; }

  ring_iterator& operator--() {
    auto tmp = m_ptr - 1;

    if (m_parent->m_head < m_parent->m_tail) {
      tmp = (tmp < m_parent->m_data) ? m_parent->m_end - 1 : tmp;
    }
    m_ptr = (tmp < m_parent->m_tail) ? m_parent->m_tail : tmp;

    return *this;
  }

  ring_iterator& operator++() {
    auto tmp = m_ptr + 1;

    if (m_parent->mHead < m_parent->m_tail) {
      tmp = (tmp >= m_parent->m_end) ? m_parent->m_data : tmp;
    }
    m_ptr = (tmp == m_parent->m_head) ? m_ptr : tmp;

    return *this;
  }

  ring_iterator operator++(int) {
    ring_iterator tmp = *this; ++(*this); return tmp;
  }

  ring_iterator operator--(int) {
    ring_iterator tmp = *this; --(*this); return tmp;
  }

  friend bool operator== (const ring_iterator& aLsh, const ring_iterator& aRsh) {
    return aLsh.m_ptr == aRsh.m_ptr;
  };

  friend bool operator!= (const ring_iterator& aLsh, const ring_iterator& aRsh) {
    return aLsh.m_ptr != aRsh.m_ptr;
  };

private:
  parent* m_parent;
  pointer m_ptr;
};






template <typename T> class ring {
public:
  using value_type = std::decay_t<T>;
  using iterator = ring_iterator<ring>;
  static_assert( std::is_same_v< T, value_type >);

  ring( value_type* aBuffer, const std::size_t aSize )
    : m_data(aBuffer)
    , m_end(aBuffer + aSize)
    , m_max_size(aSize)
    , m_head(aBuffer)
    , m_tail(aBuffer)
    , m_full(false)
  {
  }

  iterator begin() { return ring_iterator(this, m_tail); }
  iterator end() { return ring_iterator(this, m_head); }
  inline bool full() { return m_full; }
  inline bool empty() { return ( m_head == m_tail) && !m_full; }
  inline std::size_t max_size() { return m_end - m_data; }
  inline void clear() { m_head = m_data; m_tail = m_head; }

  std::size_t size() {
    std::size_t size = max_size();

    if (!m_full) {
      if ( m_head >= m_tail )
      {
        size = m_head - m_tail;
      }
      else
      {
        size += ( m_head - m_tail );
      }
    }

    return size;
  }

  void pop_front() {
    if ( !empty() )
    {
      m_full = false;

      if ( ++m_tail == m_end )
      {
        m_tail = m_data;
      }
    }

    return;
  }

  void pop_back() {
    if ( !empty() ) {
      m_full = false;

      if ( --m_head < m_data )
      {
        m_head = m_end - 1;
      }
    }

    return;
  }

  template < typename K > void push_back( K&& data ) {
    Assign( std::forward<K>(data) );

    if ( m_full ) {
      if ( ++m_tail == m_end )
      {
        m_tail = m_data;
      }
    }

    if ( ++m_head == m_end )
    {
      m_head = m_data;
    }

    m_full = m_head == m_tail;
  }

private:
  inline void Assign( const value_type& value )
    requires std::is_copy_assignable_v<value_type>
  {
    *m_head = value;
  }

  inline void Assign( value_type&& value )
    requires std::is_move_assignable_v<value_type>
  {
    *m_head = std::move(value);
  }

private:
  const std::size_t m_max_size;
  value_type *m_data, *m_end, *m_head, *m_tail;
  bool m_full;
};






} // namespace mpp
