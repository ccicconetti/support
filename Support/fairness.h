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

#include <cmath>
#include <exception>
#include <functional>
#include <stdexcept>
#include <string>

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
 *
 * @throw std::runtime_error if the container is empty.
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

/**
 * @brief Compute weighted proportional fairness index.
 *
 * F. P. Kelly, A. K. Maulloo, and D. K. H. Tan,
 * "Rate control for communication networks: Shadow prices, proportional
 *  fairness and stability,"
 * J. of the Operational Research Society, vol. 49, no. 3, pp. 237–252, 1998
 *
 * @tparam CONTAINER_SAM the type of the container' elements (samples)
 * @tparam CONTAINER_WGH the type of the container' weights
 * @param aContainer the population
 * @param aWeights the weights (or costs), one per element, ignore if empty
 * @param aUnaryOp a unary operator to conver the elements to real values
 * @return the sum of logarithms of the samples, optionally weighted, or 0 if at
 * least on element is non-positive
 *
 * @throw std::runtime_error if there are weights and the samples and weights do
 * not have the same size.
 */
template <typename CONTAINER_SAM, typename CONTAINER_WGH>
double proportionalFairnessIndex(
    const CONTAINER_SAM&                                             aContainer,
    const CONTAINER_WGH&                                             aWeights,
    const std::function<double(typename CONTAINER_SAM::value_type)>& aUnaryOp =
        [](const auto& aValue) { return aValue; }) {
  if (not aWeights.empty() and aWeights.size() != aContainer.size()) {
    throw std::runtime_error(
        "inconsistent sizes when calculating proportional fairness index: " +
        std::to_string(aContainer.size()) + " samples vs. " +
        std::to_string(aWeights.size()) + " weights");
  }
  double ret = 0;
  auto   it  = aContainer.begin();
  auto   jt  = aWeights.begin();
  while (it != aContainer.end()) {
    auto myWeight = 1.0;
    if (jt != aWeights.end()) {
      myWeight = *jt;
      ++jt;
    }
    const auto myValue = aUnaryOp(*it);
    if (not std::isnormal(myValue) or myValue < 0) {
      return 0;
    }
    ret += myWeight * std::log(myValue);
    ++it;
  }
  return ret;
} // namespace support

} // namespace support
} // namespace uiiit
