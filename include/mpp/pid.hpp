#pragma once

#include <limits>
#include <mpp/pipeblock.hpp>

namespace mpp::pid {

struct Factors {
  float Kp;
  float Ki;
  float Kd;
  float Imax;
  float Imin;
};

// TODO: регуляторы должны работать в целочисленном режиме
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
    , mImax(std::numeric_limits<decltype(mImax)>::max())
    , mImin(std::numeric_limits<decltype(mImin)>::lowest())
    , mIsum()
    , mPreviousError()
    , mTarget() { }


  Regulator( float aKp, float aKi, float aKd, float aImax, float aImin )
    : mKp(aKp)
    , mKi(aKi)
    , mKd(aKd)
    , mImax(aImax)
    , mImin(aImin)
    , mIsum()
    , mPreviousError()
    , mTarget() { }


  Regulator( const Factors&& aFactors )
    : mKp(aFactors.Kp)
    , mKi(aFactors.Ki)
    , mKd(aFactors.Kd)
    , mImax(aFactors.Imax)
    , mImin(aFactors.Imin)
    , mIsum()
    , mPreviousError()
    , mTarget() { }


  void SetTarget( output_type aTarget ) {
    mTarget = aTarget;
  }

  output_type GetTarget() {
    return mTarget;
  }

  void Reset() {
    mIsum = decltype(mIsum)();
    mPreviousError = decltype(mPreviousError)();
  }

  void Input( input_type aValue ) override {
    float error = mTarget - aValue;
    mIsum += error;
    mIsum = (mIsum > mImax) ? mImax :
            (mIsum < mImin) ? mImin : mIsum;

    float P = error * mKp;
    float I = mIsum * mKi;
    float D = (error - mPreviousError) * mKd;

    block_type::mOut = P+I+D;
    mPreviousError = error;

    return;
  }

  Regulator<input_type>& operator[](input_type aValue) {
    mTarget = aValue;
    return *this;
  }

private:
  float mKp;
  float mKi;
  float mKd;
  float mImax;
  float mImin;
  float mIsum;
  output_type mPreviousError;
  output_type mTarget;
};


} // namespace mpp::pid
