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

#include "stat.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/weighted_mean.hpp>

namespace bacc = boost::accumulators;

namespace uiiit {
namespace support {

struct Accumulator final {
  Accumulator()
      : theObj() {
  }

  ~Accumulator() {
  }

  bacc::accumulator_set<SummaryStat::Real,
                        bacc::stats<bacc::tag::mean,
                                    bacc::tag::variance,
                                    bacc::tag::min,
                                    bacc::tag::max>>
      theObj;
};

SummaryStat::SummaryStat()
    : theAcc(new Accumulator()) {
}

void SummaryStat::operator()(const Real aValue) {
  theAcc->theObj(aValue);
}

void SummaryStat::reset() {
  theAcc = std::make_unique<Accumulator>();
}

SummaryStat::Real SummaryStat::mean() const {
  return bacc::mean(theAcc->theObj);
}

SummaryStat::Real SummaryStat::min() const {
  return bacc::min(theAcc->theObj);
}

SummaryStat::Real SummaryStat::max() const {
  return bacc::max(theAcc->theObj);
}

bool SummaryStat::empty() const {
  return bacc::count(theAcc->theObj) == 0;
}

size_t SummaryStat::count() const {
  return bacc::count(theAcc->theObj);
}

SummaryStat::Real SummaryStat::stddev() const {
  return sqrt(bacc::variance(theAcc->theObj));
}

struct WeightedAccumulator final {
  WeightedAccumulator()
      : theObj() {
  }

  ~WeightedAccumulator() {
  }

  bacc::accumulator_set<double,
                        bacc::stats<bacc::tag::weighted_mean, bacc::tag::count>,
                        double>
      theObj;
};

SummaryWeightedStat::SummaryWeightedStat(const double& aClock,
                                         const double  aWarmUp)
    : theClock(aClock)
    , theWarmUp(aWarmUp)
    , theLast(0)
    , theAcc(new WeightedAccumulator()) {
  // noop
}

void SummaryWeightedStat::operator()(const double aValue) {
  if (theClock >= theWarmUp) {
    theAcc->theObj(aValue, bacc::weight = (theClock - theLast));
    theLast = theClock;
  }
}

double SummaryWeightedStat::mean() {
  return bacc::weighted_mean(theAcc->theObj);
}

size_t SummaryWeightedStat::count() const {
  return bacc::count(theAcc->theObj);
}

} // namespace support
} // namespace uiiit
