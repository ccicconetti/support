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

#include "movingvariance.h"

namespace uiiit {
namespace support {

MovingVariance::MovingVariance(const size_t aSize)
    : Base(aSize)
    , theM2n(0)
    , theMean(0) {
  if (aSize < 2) {
    throw ZeroWindow();
  }
}

MovingVariance::~MovingVariance() {
  // nothing
}

void MovingVariance::add(const double aValue) {
  if (empty()) {
    assert(theM2n == 0);
    assert(not full());
    theMean = aValue;
    Base::add(aValue);
    return;
  }

  if (full()) {
    const auto N = size();

    // remove the oldest value
    const auto myOldestValue = pop();

    // save the mean value for later
    const auto myPrevMean = theMean;

    // this is the mean if the oldest value was not added
    assert(N > 1);
    theMean = (theMean * N - myOldestValue) / (N - 1);

    // update the M_2n
    theM2n -= (myOldestValue - theMean) * (myOldestValue - myPrevMean);
  }

  assert(not full());

  const auto N = size();

  // mean without the new value
  const auto myPrevMean = theMean;

  // mean with the new value
  theMean = (theMean * N + aValue) / (N + 1);

  // update the M_2n
  theM2n += (aValue - theMean) * (aValue - myPrevMean);

  // add the value to the window
  Base::add(aValue);
}

double MovingVariance::variance() const noexcept {
  const auto N = size();
  return N == 0 ? 0.0 : std::max(0.0, (theM2n / N));
}

} // namespace support
} // namespace uiiit
