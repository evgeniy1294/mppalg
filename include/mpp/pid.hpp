#pragma once

#include <limits>
#include <mpp/pipeblock.hpp>

namespace mpp::pid {

template< typename T >
class Regulator: public PipeBlock< float, T >
{
public:
  using output_type = float;
  using input_type  = std::decay_t<T>;
  using block_type  = PipeBlock< float, T >;

  Regulator( float aKp, float aKi, float aKd )
    : mKp(aKp)
    , mKi(aKi)
    , mKd(aKd)
    , mOutputMax(std::numeric_limits<decltype(mOutputMax)>::max())
    , mOutputMin(std::numeric_limits<decltype(mOutputMin)>::lowest())
    , mImax(std::numeric_limits<decltype(mImax)>::max())
    , mImin(std::numeric_limits<decltype(mImin)>::lowest())
    , mIsum()
    , mDfeedback() { }


  void SetTarget( output_type aTarget ) {
    mTarget = aTarget;
  }

  void Input( input_type aValue ) override {
    float error = mTarget - aValue;
    mIsum += error;
    mIsum = (mIsum > mImax) ? mImax :
            (mIsum < mImin) ? mImin : mIsum;

    float P = error * mKp;
    float I = mIsum * mKi;
    float D = (error - mDfeedback) * mKd;
    mDfeedback = error;

    block_type::mOut = P+I+D;
    block_type::mOut = (block_type::mOut > mOutputMax) ? mOutputMax :
                       (block_type::mOut < mOutputMin) ? mOutputMin : block_type::mOut;

    return;
  }

  Regulator<input_type>& operator[](input_type aValue) {
    mTarget = aValue;
    return *this;
  }

private:
  output_type mKp;
  output_type mKi;
  output_type mKd;
  output_type mOutputMin;
  output_type mOutputMax;
  output_type mImax;
  output_type mImin;
  output_type mIsum;
  output_type mDfeedback;
  output_type mTarget;
};


} // namespace mpp::pid
