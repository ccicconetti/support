/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2019 Claudio Cicconetti https://ccicconetti.github.io/

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
