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
