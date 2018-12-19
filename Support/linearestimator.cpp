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
