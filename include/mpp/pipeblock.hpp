#pragma once

#include <type_traits>
#include <utility>

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

  PipeBlock<O, I>& operator<<( input_type aValue ) {
    Input(aValue);
    return *this;
  }

  operator output_type() {
    return mOut;
  }
protected:
  output_type mOut;
};


template <class = void> inline constexpr static auto is_pipe_block_v = false;
template <class O, class I> inline constexpr static bool is_pipe_block_v<mpp::PipeBlock<O, I>> = true;


template< typename O, typename I >
auto& operator>>(auto&& aInput, mpp::PipeBlock<O, I>& aPipeBlock)
{
  if constexpr (is_pipe_block_v<::std::decay_t<decltype(aInput)>>)
  {
     aPipeBlock.Input(aInput.Output());
  }
  else
  {
    static_assert(std::is_convertible_v<decltype(aInput), I> );
    aPipeBlock.Input(std::forward<decltype(aInput)>(aInput));
  }

  return aPipeBlock;
}


} // namespace mpp
