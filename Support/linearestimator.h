/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2018 Claudio Cicconetti <https://about.me/ccicconetti>

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
