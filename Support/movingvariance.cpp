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
