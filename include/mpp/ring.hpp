#pragma once

/**
 * @file
 *   This file defines ring buffer and iterator classes for framework
 */

#include <cstdint>
#include <optional>
#include <type_traits>
#include <iterator>
#include <cstddef>
#include <cstring>

//#include <mpp/assert.h>


namespace mpp {

template< class T >
class ring_iterator {
  static_assert(std::is_same_v<T, std::decay_t<T>>);

public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type   = std::ptrdiff_t;
  using value_type        = typename T::value_type;
  using pointer           = typename T::pointer;
  using reference         = typename T::reference;
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
    if (m_ptr != m_parent->m_tail) {
      m_ptr--;
    }

    if (m_parent->m_head < m_parent->m_tail) {
      m_ptr = (m_ptr < m_parent->m_data) ? m_parent->m_end - 1 : m_ptr;
    }

    return *this;
  }

  ring_iterator& operator++() {
    if (m_ptr != m_parent->m_head) {
      m_ptr++;
    }

    if (m_parent->m_head < m_parent->m_tail) {
      m_ptr = (m_ptr >= m_parent->m_end) ? m_parent->m_data : m_ptr;
    }

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
  friend class ring_iterator<ring>;
public:
  using value_type = std::decay_t<T>;
  using pointer    = value_type*;
  using reference  = value_type&;
  using const_reference = const value_type&;
  using iterator = ring_iterator<ring>;
  static_assert( std::is_same_v< T, value_type >);


  ring( pointer a_buffer, const std::size_t a_size )
    : m_data(a_buffer)
    , m_end(a_buffer + a_size)
    , m_head(a_buffer)
    , m_tail(a_buffer)
    , m_full(false)
  {
  }

  ring( pointer a_buffer, pointer a_end )
    : m_data(a_buffer)
    , m_end(a_end)
    , m_head(a_buffer)
    , m_tail(a_buffer)
    , m_full(false)
  {
  }

  reference front() { return *m_tail; }
  const_reference front() const { return *m_tail; }
  reference back() { auto tmp = m_head - 1; return (tmp < m_data) ? m_end - 1 : tmp; }
  const_reference back() const { return back(); }
  iterator begin() { return ring_iterator(this, m_tail); }
  iterator end() { return ring_iterator(this, m_head); }
  inline bool full() { return m_full; }
  inline bool empty() { return m_head == m_tail; }
  inline std::size_t max_size() { return m_end - m_data - 1; }
  inline void clear() { m_head = m_data; m_tail = m_head; }

  std::size_t size() {
    std::size_t size = max_size();

    if (!m_full) {
      size = (m_head >= m_tail) ? m_head - m_tail : size + (m_head - m_tail);
    }

    return size;
  }

  void pop_front() {
    if ( !empty() )
    {
      m_full = false;
      m_tail = (++m_tail == m_end) ? m_data : m_tail;
    }

    return;
  }

  void pop_back() {
    if ( !empty() ) {
      m_full = false;
      m_head = (--m_head < m_data) ? m_end - 1 : m_head;
    }

    return;
  }

  template < typename K > void push_back( K&& data ) {
    Assign( std::forward<K>(data) );

    if ( m_full ) {
      m_tail = (++m_tail == m_end) ? m_data : m_tail;
    }
    m_head = (++m_head == m_end) ? m_data : m_head;

    if (m_head == m_tail) {
      m_full = true;
      m_tail = (++m_tail == m_end) ? m_data : m_tail;
    }
  }

  void push_back(pointer a_data, pointer a_end) {
    if (a_end > a_data) {
      std::size_t size = a_end - a_data;

      if ( size >= max_size() ) {
        a_data = a_end - max_size();
        memcpy(m_data, a_data, max_size());
        m_tail = m_data;
        m_head = m_end - 1;
      }
      else
      {
        auto insert_ptr = m_head;
        auto insert_end = insert_ptr + size;
        insert_end = (insert_end > m_end) ? insert_end - m_end : insert_end;

        while(insert_ptr != insert_end) {
          *insert_ptr++ = *a_data++;
          insert_ptr = (insert_ptr == m_end) ? m_data : insert_ptr;
        }

        if ( is_belong(m_tail, m_head, insert_ptr) ) {
          m_tail = insert_ptr + 1;
          m_tail = (m_tail == m_end) ? m_data : m_tail;
        }
        m_head = (insert_ptr == m_end) ? m_data : insert_ptr;

        if (m_tail == m_head) {
          m_full = true;
          m_tail = (++m_tail == m_end) ? m_data : m_tail;
        }
      }
    }
    return;
  }



private:
  inline void Assign( const_reference value )
    requires std::is_copy_assignable_v<value_type>
  {
    *m_head = value;
  }

  inline void Assign( value_type&& value )
    requires std::is_move_assignable_v<value_type>
  {
    *m_head = std::move(value);
  }

  inline bool is_belong(pointer ptr, pointer first, pointer last) {
    MPP_ASSERT(ptr   >= m_data && ptr   < m_end);
    MPP_ASSERT(first >= m_data && first < m_end);
    MPP_ASSERT(last  >= m_data && last  < m_end);

    bool ret = false;

    if (first < last) {
      // <---first++++++++++last---->
      ret = ptr >= first && ptr <= last;
    }
    else if (first > last) {
      // <++++last---------first++++>
      ret = ptr <= last || ptr >= first;
    }

    return ret;
  }

private:
  pointer m_data, m_end, m_head, m_tail;
  bool m_full;
};






} // namespace mpp
