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

#include <cassert>
#include <set>
#include <type_traits>
#include <utility>

namespace uiiit {
namespace support {

/**
 * Fit a set of points to a straight line a+bx.
 * It is assumed that all the points have the same weight.
 * Adaptation of the algorithm in "Numerical Recipes in C", 2nd edition,
 * Chap. 15.2 Fitting Data to a Straight Line.
 *
 * \param[in] aPoints The input values.
 *
 * \return The a,b coefficients of the fitted line.
 *
 * Special cases:
 * - empty container: return slope = intercept = 0;
 * - a single point is passed: return slope = 0, intercept = y-axis value
 *   of input points.
 * - all the points have the same abscissa: return slope = 0, intercept =
 *   average of the y-axis values of the input points.
 *
 * CONTAINER is a an iterable container of pairs containing the x- and y-axis
 * values. The first and second type of the pair must be the same. The type of
 * the slope/intercept values returned is the same as that of the points.
 */
template <class CONTAINER>
typename CONTAINER::value_type fit(const CONTAINER& aPoints) noexcept {
  static_assert(
      std::is_same<typename CONTAINER::value_type::first_type,
                   typename CONTAINER::value_type::second_type>::value,
      "x- and y-axis values must use the same type");

  using Type = typename CONTAINER::value_type::first_type;

  // special case: empty dataset
  if (aPoints.empty()) {
    return std::make_pair(0, 0);
  }

  std::set<Type> myDiffValues;
  Type           sx(0);
  Type           sy(0);
  for (const auto& myPoint : aPoints) {
    if (myDiffValues.size() <= 1) {
      myDiffValues.insert(myPoint.first);
    }
    sx += myPoint.first;
    sy += myPoint.second;
  }

  // special case: all points with the same abscissa (or a single point)
  assert(not myDiffValues.empty());
  if (myDiffValues.size() == 1) {
    assert(not aPoints.empty());
    return std::make_pair(sy / aPoints.size(), 0);
  }

  // typical case: perform line regression
  const Type sxoss = sx / aPoints.size();
  Type       st2(0);
  Type       b(0);
  for (const auto& myPoint : aPoints) {
    const Type t = myPoint.first - sxoss;
    st2 += t * t;
    b += t * myPoint.second;
  }
  assert(st2 > 0);
  b /= st2;
  const Type a = (sy - sx * b) / aPoints.size();
  return std::make_pair(a, b);
}

/**
 * Find the slope/intercept of the line a + bx passing for the two points below.
 *
 * \return The a,b coefficients of the fitted line, or (0,0) if x1 == x2.
 */
template <class TYPE>
std::pair<TYPE, TYPE>
fit(const TYPE x1, const TYPE y1, const TYPE x2, const TYPE y2) {
  // corner case: not possible to fit because of too many degrees of freedom
  if (x1 == x2) {
    return std::make_pair(0, 0);
  }

  return std::make_pair((x2 * y1 - x1 * y2) / (x2 - x1), (y1 - y2) / (x1 - x2));
}

/**
 * Find the line passing for the two points given, then interpolate/extrapolate
 * the value on the y-axis of the given x.
 *
 * \return y that (x,y) lies on the line a+bx passing for (x1,y2) and (x2,y2).
 *
 * If x1 == x2 then return 0.
 */
template <class TYPE>
TYPE extrapolate(
    const TYPE x1, const TYPE y1, const TYPE x2, const TYPE y2, const TYPE x) {
  const auto ret = fit<TYPE>(x1, y1, x2, y2);
  return ret.first + ret.second * x;
}

} // namespace support
} // namespace uiiit
