#pragma once

/**
 * @file
 *   This file defines ring buffer and iterator classes for framework.
 */

#include <cstdint>
#include <optional>
#include <type_traits>
#include <iterator>
#include <cstddef>
#include <cstring>

#include <mpp/macro_assert.h>


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

  /**
   * @brief This constructor creates a ring_iterator instance.
   *
   * @param[in]  a_parent A pointer to the parent class instance.
   * @param[in]  a_ptr    A pointer to a data.
   *
  */
  ring_iterator(parent* a_parent, pointer a_ptr)
    : m_parent(a_parent)
    , m_ptr(a_ptr) {}


  /**
   * @brief This constructor creates a ring_iterator instance.
   *
   * @param[in]  a_parent A pointer to the parent class instance.
   * @param[in]  a_ptr    A pointer to a data.
   *
  */
  ring_iterator(T& a_parent)
    : m_parent(&a_parent)
    , m_ptr(a_parent.m_tail) {}


  /**
   * @brief This method indicates whether or not the iterator equal ring head.
   *
   * @retval TRUE   If iterator is equal ring head.
   * @retval FALSE  If iterator is not equal ring head.
   *
  */
  bool is_head() { return m_ptr == m_parent->m_head; }

  /**
   * @brief This method indicates whether or not the iterator equal ring tail.
   *
   * @retval TRUE   If iterator is equal ring tail.
   * @retval FALSE  If iterator is not equal ring tail.
   *
  */
  bool is_tail() { return m_ptr == m_parent->m_tail; }

  reference operator*() const { return *m_ptr; }
  pointer operator->() { return m_ptr; }

  /**
   * @brief Prefix decrement.
  */
  ring_iterator& operator--() {
    if (m_ptr != m_parent->m_tail) {
      m_ptr--;
    }

    if (m_parent->m_head < m_parent->m_tail) {
      m_ptr = (m_ptr < m_parent->m_data) ? m_parent->m_end - 1 : m_ptr;
    }

    return *this;
  }

  /**
   * @brief Prefix increment.
  */
  ring_iterator& operator++() {
    if (m_ptr != m_parent->m_head) {
      m_ptr++;
    }

    if (m_parent->m_head < m_parent->m_tail) {
      m_ptr = (m_ptr >= m_parent->m_end) ? m_parent->m_data : m_ptr;
    }

    return *this;
  }

  /**
   * @brief Postfix increment.
  */
  ring_iterator operator++(int) {
    ring_iterator tmp = *this; ++(*this); return tmp;
  }

  /**
   * @brief Postfix decrement.
  */
  ring_iterator operator--(int) {
    ring_iterator tmp = *this; --(*this); return tmp;
  }

  friend bool operator== (const ring_iterator& aLsh, const ring_iterator& aRsh) {
    return aLsh.m_ptr == aRsh.m_ptr;
  };

  friend bool operator!= (const ring_iterator& aLsh, const ring_iterator& aRsh) {
    return aLsh.m_ptr != aRsh.m_ptr;
  };

  operator pointer() { return m_ptr; }
private:
  parent* m_parent;
  pointer m_ptr;
};





/**
 * @brief STL-compatibility embedded ring-buffer
 * @note  The elements can be accessed only through iterators
*/
template <typename T> class ring
{
  friend class ring_iterator<ring>;
public:
  using value_type = std::decay_t<T>;
  using pointer    = value_type*;
  using reference  = value_type&;
  using const_reference = const value_type&;
  using iterator = ring_iterator<ring>;
  static_assert( std::is_same_v< T, value_type >);


  /**
   * @brief This constructor creates a ring buffer instance.
   *
   * @param[in]  a_buffer A pointer to the buffer.
   * @param[in]  a_size   A buffer size.
   *
  */
  ring( pointer a_buffer, const std::size_t a_size )
    : m_data(a_buffer)
    , m_end(a_buffer + a_size)
    , m_head(a_buffer)
    , m_tail(a_buffer)
    , m_full(false)
  {
  }

  /**
   * @brief This constructor creates a ring buffer instance.
   *
   * @param[in]  a_buffer A pointer to the buffer.
   * @param[in]  a_end    A pointer to the end buffer.
   *
  */
  ring( pointer a_buffer, pointer a_end )
    : m_data(a_buffer)
    , m_end(a_end)
    , m_head(a_buffer)
    , m_tail(a_buffer)
    , m_full(false)
  {
  }

  /**
   * @brief Access the first element.
   * @note Calling front on an empty container is undefined.
  */
  reference front() { return *m_tail; }
  const_reference front() const { return *m_tail; }

  /**
   * @brief Access the last element.
   * @note Calling back on an empty container is undefined.
  */
  reference back() { auto tmp = m_head - 1; return (tmp < m_data) ? m_end - 1 : tmp; }
  const_reference back() const { return back(); }

  /**
   * @brief Returns an iterator to the beginning.
   */
  iterator begin() { return ring_iterator(this, m_tail); }

  /**
   * @brief Returns an iterator to the end.
  */
  iterator end() { return ring_iterator(this, m_head); }

  /**
   * @brief checks whether the container is full.
   *
   * @retval TRUE   If buffer is full.
   * @retval FALSE  If buffer is not full.
   *
  */
  inline bool full() { return m_full; }

  /**
   * @brief Checks whether the container is empty
   *
   * @retval TRUE   If buffer is empty.
   * @retval FALSE  If buffer is not empty.
   *
  */
  inline bool empty() { return m_head == m_tail; }

  /**
   * @brief Returns the maximum possible number of elements
  */
  inline std::size_t max_size() { return m_end - m_data - 1; }

  /**
   * @brief Clears the contents
  */
  inline void clear() { m_head = m_data; m_tail = m_head; }

  /**
   * @brief Returns the number of elements
  */
  std::size_t size() {
    std::size_t size = max_size();

    if (!m_full) {
      size = (m_head >= m_tail) ? m_head - m_tail : size + (m_head - m_tail);
    }

    return size;
  }

  /**
   * @brief Move ring logic to another buffer
  */
  void remap(pointer a_buffer, pointer a_end) {
    m_data = a_buffer;
    m_end  = a_end;
    m_head = a_buffer;
    m_tail = a_buffer;
    m_full = false;
  }

  void remap(pointer a_buffer, const std::size_t a_size) {
    m_data = a_buffer;
    m_end  = a_buffer + a_size;
    m_head = a_buffer;
    m_tail = a_buffer;
    m_full = false;
  }

  /**
   * @brief Removes the first element
  */
  void pop_front() {
    if ( !empty() )
    {
      m_full = false;
      m_tail = (++m_tail == m_end) ? m_data : m_tail;
    }

    return;
  }


  /**
   * @brief Removes the last element
  */
  void pop_back() {
    if ( !empty() ) {
      m_full = false;
      m_head = (--m_head < m_data) ? m_end - 1 : m_head;
    }

    return;
  }


  /**
   * @brief Adds an element to the end
   *
   * @param[in]  data A reference to data.
  */
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

  /**
   * @brief Adds an elements to the end
   *
   * @param[in]  a_data A pointer to the buffer.
   * @param[in]  a_end  A pointer to the end of buffer.
  */
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
        insert_end = (insert_end > m_end) ? m_data + (insert_end - m_end) : insert_end;

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

  /**
   * @brief Adds an elements to the end
   * @note Очень опасная функция, переписать.
  */
  void push_from(iterator& a_iter, pointer a_data, pointer a_end) {
    m_head = a_iter;
    MPP_ASSERT(m_head >= m_data && m_head < m_end);

    push_back(a_data, a_end);
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
