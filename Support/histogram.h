#pragma once

#include "macros.h"
#include "stat.h"

#include <ostream>
#include <vector>

namespace uiiit {
namespace support {

/**
 * Class implemented a binned histogram.
 * For each bin we keep a SummaryStat object that provides basic statistics on
 * the weights added.
 *
 * An underflow and an overflow bin are kept for the values outside the domain.
 */
class Histogram final
{
  MOVEONLY(Histogram);

 public:
  using Real = support::SummaryStat::Real;

  enum OverflowPolicy {
    KEEP   = 0,
    IGNORE = 1,
    THROW  = 2,
  };

  /**
   * \param aLower the lower bound of the domain.
   * \param aBinSpan the width of the bin.
   * \param aNumBins the number of bins.
   * \param aPolicy what to do with values outside the domain.
   *
   * \throw std::runtime_error if the number of bins is 0 or if the the bin span
   * is not positive.
   */
  explicit Histogram(const Real           aLower,
                     const Real           aBinSpan,
                     const size_t         aNumBins,
                     const OverflowPolicy aPolicy);

  //! Added a new value, with weight.
  void operator()(const Real aValue, const Real aWeight);

  /**
   * \return the statistics of a regular bin
   *
   * \throw std::runtime_error if the value is outside the domain.
   */
  SummaryStat& stat(const Real aValue);

  /**
   * \param aMinZero If true then we assume that a lower bound 0,0 exists.
   *
   * \return the mean of the values in a regular bin. If the corresponding bin
   * does not have values, then look for the closest non-empty lower and upper
   * bins and use them to extrapolate the return value using a 2-point linear
   * fit. If the lower or upper value are undefined, then return the only
   * defined value. If all the regular bins in the histogram are empty, then
   * return 0.
   *
   * \throw std::runtime_error if the value is outside the domain.
   */
  Real closestMean(const Real aValue, const bool aMinZero) const;

  //! \return the underflow stat or throw if not available.
  SummaryStat& underflow();

  //! \return the overflow stat or throw if not available.
  SummaryStat& overflow();

  //! Print a multi-line text representation of the histogram.
  void print(std::ostream& aStream);

 private:
  /**
   * \return the index of the bin corresponding to the given value.
   */
  ssize_t binNdx(const Real aValue) const noexcept;

  /**
   * \throw std::runtime_error if the value is outside the domain.
   */
  void throwIfOutside(const ssize_t aBinNdx) const;

  //! \return the value corresponding to the middle of the given bin.
  Real binValue(const ssize_t aBinNdx) const noexcept;

  //! \return a single-line human-readable representation of the domain.
  std::string domain() const;

 private:
  // const configuration from ctor
  const Real           theLower;
  const Real           theBinSpan;
  const OverflowPolicy thePolicy;

  // internal state
  std::vector<SummaryStat> theStats;
  SummaryStat              theUnderflow;
  SummaryStat              theOverflow;
};

} // namespace support
} // namespace uiiit
