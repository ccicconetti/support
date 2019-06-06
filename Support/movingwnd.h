/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/support/
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

#include "chrono.h"
#include "macros.h"
#include "movingexceptions.h"

#include <cassert>
#include <vector>

namespace uiiit {
namespace support {

/**
 * A moving window of values with an associated timestamp for clean-up.
 *
 * TYPE must be default-constructible and copyable.
 */
template <class TYPE>
class MovingWnd
{
  MOVEONLY(MovingWnd);

 public:
  /**
   * Create a moving window with no values stored.
   *
   * \throw ZeroWindow is aSize == 0.
   */
  explicit MovingWnd(const size_t aSize);

  virtual ~MovingWnd();

  //! Restore the structure to the initial value (empty).
  void clear() noexcept;

  //! \return true if the window is empty.
  bool empty() const noexcept;

  //! \return true if adding a new value would remove an existing one.
  bool full() const noexcept;

  //! \return the number of elements in the window.
  size_t size() const noexcept;

  //! Add a new value.
  void add(const TYPE aValue) noexcept;

  //! Pop the oldest value or throw EmptyWindow is there are no values.
  TYPE pop();

  //! \return a copy of the values.
  std::vector<TYPE> values() const noexcept;

  /**
   * Remove all values older than the specified time, in seconds.
   *
   * \return true if at least only value was removed.
   */
  bool purge(const double aInterval);

 private:
  support::Chrono     theChrono;
  std::vector<TYPE>   theValues;
  std::vector<double> theTimestamps;
  size_t              theCur;
  size_t              theTot;
};

template <class TYPE>
MovingWnd<TYPE>::MovingWnd(const size_t aSize)
    : theChrono(true)
    , theValues(aSize)
    , theTimestamps(aSize)
    , theCur(0)
    , theTot(0) {
  if (aSize == 0) {
    throw ZeroWindow();
  }
}

template <class TYPE>
MovingWnd<TYPE>::~MovingWnd() {
}

template <class TYPE>
void MovingWnd<TYPE>::clear() noexcept {
  theTot = 0;
}

template <class TYPE>
bool MovingWnd<TYPE>::empty() const noexcept {
  return theTot == 0;
}

template <class TYPE>
bool MovingWnd<TYPE>::full() const noexcept {
  return theTot == theValues.size();
}

template <class TYPE>
size_t MovingWnd<TYPE>::size() const noexcept {
  return theTot;
}

template <class TYPE>
void MovingWnd<TYPE>::add(const TYPE aValue) noexcept {
  const auto N = theValues.size();
  assert(theValues.size() == theTimestamps.size());
  assert(theTot <= N);
  if (theTot < N) {
    theTot++;
  }
  theValues[theCur]     = aValue;
  theTimestamps[theCur] = theChrono.time();
  theCur                = (theCur + 1) % N;
}

template <class TYPE>
TYPE MovingWnd<TYPE>::pop() {
  if (theTot == 0) {
    throw EmptyWindow();
  }

  const auto   N     = theValues.size();
  const size_t myPos = (theCur + N - theTot) % N;
  theTot--;
  return theValues[myPos];
}

template <class TYPE>
std::vector<TYPE> MovingWnd<TYPE>::values() const noexcept {
  const auto        N = theValues.size();
  std::vector<TYPE> ret(theTot);

  size_t myPos = (theCur + N - theTot) % N;
  for (size_t i = 0; i < theTot; i++) {
    ret[i] = theValues[myPos];
    myPos++;
    if (myPos == N) {
      myPos = 0;
    }
  }

  return ret;
}

template <class TYPE>
bool MovingWnd<TYPE>::purge(const double aInterval) {
  const auto N         = theValues.size();
  const auto myHorizon = theChrono.time() - aInterval;

  const auto myInitialTot = theTot;
  size_t     myPos        = (theCur + N - theTot) % N;
  for (size_t i = 0; i < theTot; i++) {
    if (theTimestamps[myPos] <= myHorizon) {
      theTot--;
    } else {
      break;
    }
    myPos++;
    if (myPos == N) {
      myPos = 0;
    }
  }

  return theTot != myInitialTot;
}

} // namespace support
} // namespace uiiit
