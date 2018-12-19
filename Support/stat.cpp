#include "stat.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>

namespace bacc = boost::accumulators;

namespace uiiit {
namespace support {

class Accumulator final
{
 public:
  Accumulator()
      : theObj() {
  }

  boost::accumulators::accumulator_set<
      SummaryStat::Real,
      boost::accumulators::stats<boost::accumulators::tag::mean,
                                 boost::accumulators::tag::variance,
                                 boost::accumulators::tag::min,
                                 boost::accumulators::tag::max>>
      theObj;
};

SummaryStat::SummaryStat()
    : theAcc(new Accumulator()) {
}

SummaryStat::~SummaryStat() {
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

} // namespace support
} // namespace uiiit
