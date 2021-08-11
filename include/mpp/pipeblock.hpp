#pragma once

#include <type_traits>

namespace mpp {

template< typename O, typename I >
class PipeBlock
{
public:
  static_assert( std::is_arithmetic_v<O>, "Output type must be arithmetic type");
  static_assert( std::is_arithmetic_v<I>, "Input type must be arithmetic type");

  using output_type = std::decay_t<O>;
  using input_type  = std::decay_t<I>;
  static_assert( std::is_same_v< O, output_type >);
  static_assert( std::is_same_v< I, input_type >);

  PipeBlock(): mOut() { }

  virtual inline void Input( input_type aValue ) {
    mOut = aValue;
  }

  virtual inline output_type Output(){
    return mOut;
  }

  virtual PipeBlock<O, I>& operator<<( input_type aValue ) {
    Input(aValue);
    return *this;
  }

  template<typename T>
  PipeBlock<T, output_type>& operator>>(PipeBlock<T, output_type>& aPipeBlock) {
    aPipeBlock << mOut;
    return aPipeBlock;
  }

  operator O() {
    return mOut;
  }
protected:
  output_type mOut;
};


} // namespace mpp
