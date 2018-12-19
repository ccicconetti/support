#pragma once

#include "macros.h"
#include "movingwnd.h"

namespace uiiit {
namespace support {

class LinearEstimator
{
  MOVEONLY(LinearEstimator);

 public:
  /**
   * \param aWindowSize the size of the moving window to keep x_i, y_i values
   * \param aStalePeriod the (real) time after which a sample is considered
   * stale; if this value is <= 0 then the samples never expire
   */
  explicit LinearEstimator(const size_t aWindowSize, const double aStalePeriod);

  /**
   * \return an extrapolation of the y-axis value based on the given x-axis
   * value; if there is not a valid fit, then return 0.
   */
  float extrapolate(const float x);

  /**
   * Add a new point to the window.
   *
   * \param x the x-axis value to be added
   * \param y the y-axis value to be added
   */
  void add(const float x, const float y);

 private:
  void update();

 private:
  using Point = std::pair<float, float>;
  support::MovingWnd<Point> theWindow;
  const double              theStalePeriod;
  float                     theIntercept;
  float                     theSlope;
  bool                      theUpdated;
};

} // namespace support
} // namespace uiiit
