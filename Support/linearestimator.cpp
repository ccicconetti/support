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

#include "linearestimator.h"

#include "fit.h"

#include <tuple>

namespace uiiit {
namespace support {

LinearEstimator::LinearEstimator(const size_t aWindowSize,
                                 const double aStalePeriod)
    : theWindow(aWindowSize)
    , theStalePeriod(aStalePeriod)
    , theIntercept(0.0f)
    , theSlope(0.0f)
    , theUpdated(true) {
}

float LinearEstimator::extrapolate(const float x) {
  update();
  return x * theSlope + theIntercept;
}

void LinearEstimator::update() {
  if (theStalePeriod > 0.0f) {
    // never purge with a non-positive stale period configured
    theUpdated &= not theWindow.purge(theStalePeriod);
  }

  // return without performing any operation if there are no new samples since
  // the last update() and no values have been removed from the window because
  // they are stale
  if (theUpdated) {
    return;
  }

  // fit the fresh values to a new slope,intercept
  std::tie(theIntercept, theSlope) = support::fit(theWindow.values());
  theUpdated                       = true;
}

void LinearEstimator::add(const float x, const float y) {
  theWindow.add({x, y});
  theUpdated = false;
}

} // namespace support
} // namespace uiiit
