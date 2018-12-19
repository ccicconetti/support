#pragma once

#include "macros.h"
#include "movingexceptions.h"

#include <vector>

namespace uiiit {
namespace support {

/**
 * Keep a moving average with fixed number of samples.
 *
 * TYPE must be default constructible and support arithmetic operations, in
 * particular sum of two values and division by unsigned integer.
 *
 * It is required that TYPE supports the sum of all the values in the window
 * without overflowing.
 */
template <class TYPE>
class MovingAvg final
{
  MOVEONLY(MovingAvg);

 public:
  /**
   * Create a moving average with no values stored.
   *
   * \throw ZeroWindow is aSize == 0.
   */
  explicit MovingAvg(const size_t aSize);

  //! Restore the structure the initial value (empty).
  void clear() noexcept;

  //! \return true if the window is empty (call to average/last throw).
  bool empty() const noexcept;

  //! Add a new value.
  void add(const TYPE aValue) noexcept;

  /**
   * \return the average of the samples so far.
   *
   * \throw EmptyWindow if empty.
   */
  TYPE average() const;

  /**
   * \return the last sample added.
   *
   * \throw EmptyWindow if empty.
   */
  TYPE last() const;

 private:
  std::vector<TYPE> theWindow;
  size_t            theCur;
  TYPE              theSum;
  size_t            theTot;
};

template <class TYPE>
MovingAvg<TYPE>::MovingAvg(const size_t aSize)
    : theWindow(aSize)
    , theCur(0)
    , theSum()
    , theTot(0) {
  if (aSize == 0) {
    throw ZeroWindow();
  }
}

template <class TYPE>
void MovingAvg<TYPE>::clear() noexcept {
  theTot = 0;
  theSum = TYPE();
}

template <class TYPE>
bool MovingAvg<TYPE>::empty() const noexcept {
  return theTot == 0;
}

template <class TYPE>
void MovingAvg<TYPE>::add(const TYPE aValue) noexcept {
  const auto N = theWindow.size();
  if (theTot == N) {
    theSum -= theWindow[theCur];
  } else {
    theTot++;
  }
  theWindow[theCur] = aValue;
  theSum += aValue;
  theCur = (theCur + 1) % N;
}

template <class TYPE>
TYPE MovingAvg<TYPE>::average() const {
  if (theTot == 0) {
    throw EmptyWindow();
  }
  return theSum / theTot;
}

template <class TYPE>
TYPE MovingAvg<TYPE>::last() const {
  if (theTot == 0) {
    throw EmptyWindow();
  }
  return theWindow[theCur == 0 ? (theWindow.size() - 1) : (theCur - 1)];
}

} // namespace support
} // namespace uiiit
