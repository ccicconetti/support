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

#include "Support/stat.h"

#include <exception>
#include <functional>
#include <stdexcept>

namespace uiiit {
namespace support {

/**
 * @brief Compute Ray Jain's fairness index
 *
 * https://en.wikipedia.org/wiki/Fairness_measure
 *
 * @tparam CONTAINER the type of the container' elements
 * @param aContainer the population
 * @param aUnaryOp a unary operator to conver the elements to real values
 * @return double
 */
template <typename CONTAINER>
double jainFairnessIndex(
    const CONTAINER& aContainer,
    const std::function<SummaryStat::Real(typename CONTAINER::value_type)>&
        aUnaryOp = [](const auto& aValue) { return aValue; }) {
  if (aContainer.empty()) {
    throw std::runtime_error(
        "cannot calculate fairness on vanishing population");
  }
  SummaryStat mySum;
  SummaryStat mySSum;
  for (const auto& elem : aContainer) {
    const SummaryStat::Real myValue = aUnaryOp(elem);
    mySum(myValue);
    mySSum(myValue * myValue);
  }
  return mySum.mean() * mySum.mean() / mySSum.mean();
}

} // namespace support
} // namespace uiiit
