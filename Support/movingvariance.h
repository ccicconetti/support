#pragma once

#include "movingwnd.h"

namespace uiiit {
namespace support {
/**
 * Compute online the variance on a moving window of samples.
 *
 * Uses Welford's formula:
 *
 * M_2n = M_2n-1 + (x_n - u_n-1) (x_n - u_n)
 *
 * variance = M_2n / (n - 1)
 *
 * where:
 *
 * u_n-1 = sample mean without n-th value
 * u_n = sample mean with the n-th value
 */
class MovingVariance final : private MovingWnd<double>
{
  using Base = MovingWnd<double>;

 public:
  using Base::empty;

  /**
   * \param aSize the number of elements in the window.
   *
   * \throw ZeroWindow if aSize == 0
   */
  explicit MovingVariance(const size_t aSize);

  ~MovingVariance() override;

  //! Add a new value to the window.
  void add(const double aValue);

  /**
   * \return the variance.
   *
   * If there are less than two points in the window, return 0.
   */
  double variance() const noexcept;

 private:
  double theM2n;
  double theMean;
};

} // namespace support
} // namespace uiiit
