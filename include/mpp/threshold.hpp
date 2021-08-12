#pragma once

#include <limits>
#include <mpp/pipeblock.hpp>


namespace mpp {

template<typename T>
class ThresholdBlock: public PipeBlock<T, T>
{
public:
  using value_type = std::decay_t<T>;
  using block_type = PipeBlock<T, T>;

  ThresholdBlock()
    : mMin(std::numeric_limits<value_type>::lowest())
    , mMax(std::numeric_limits<value_type>::max())
    , mFallLimit(std::numeric_limits<value_type>::max())
    , mRiseLimit(std::numeric_limits<value_type>::max()) { }

  ThresholdBlock(value_type aMin, value_type aMax)
    : mMin(aMin)
    , mMax(aMax)
    , mFallLimit(std::numeric_limits<value_type>::max())
    , mRiseLimit(std::numeric_limits<value_type>::max()) { }

  ThresholdBlock(value_type aMin, value_type aMax, value_type aFallLimit, value_type aRiseLimit)
    : mMin(aMin)
    , mMax(aMax)
    , mFallLimit(aFallLimit)
    , mRiseLimit(aRiseLimit) { }

  void SetThreshold(value_type aMin, value_type aMax) {
    mMin = aMin;
    mMax = aMax;
  }

  void SetMin(value_type aMin) {
    mMin = aMin;
  }

  void SetMax(value_type aMax) {
    mMax = aMax;
  }

  void SetLimits(value_type aFallLimit, value_type aRiseLimit) {
    mFallLimit = aFallLimit;
    mRiseLimit = aRiseLimit;
  }

  void SetRiseLimit(value_type aRiseLimit) {
    mRiseLimit = aRiseLimit;
  }

  void SetFallLimit(value_type aFallLimit) {
    mFallLimit = aFallLimit;
  }

  void Input(value_type aValue) override {
    value_type tmp = (aValue < mMin) ? mMin :
                     (aValue > mMax) ? mMax : aValue;

    if (tmp >= block_type::mOut) {
      block_type::mOut = ((tmp - block_type::mOut) > mRiseLimit) ?
                              block_type::mOut + mRiseLimit : tmp;
    }
    else
    {
      block_type::mOut = ((block_type::mOut - tmp) > mFallLimit) ?
                              block_type::mOut - mFallLimit : tmp;
    }
  }

  ThresholdBlock<value_type>& operator[](value_type aValue) {
    block_type::mOut = aValue;
    return *this;
  }

private:
  value_type mMin;
  value_type mMax;
  value_type mRiseLimit;
  value_type mFallLimit;
};

} // namespace mpp
