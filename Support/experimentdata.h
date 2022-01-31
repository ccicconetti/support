/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2022 Claudio Cicconetti https://ccicconetti.github.io/

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

#include "Support/chrono.h"
#include "Support/macros.h"

#include <glog/logging.h>

#include <cstddef>
#include <deque>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <tuple>
#include <utility>

namespace uiiit {
namespace support {

/**
 * @brief Keeps experiment data in memory. Also records the experiment duration.
 *
 * @tparam IN the description of the experiment
 * @tparam OUT the output of the experiment
 *
 * Both IN and OUT must be movable and default-constructible, and they must
 * implement the following members:
 *
 * - toCsv(): return a single-line string of comma-separated values
 * - toString(): return a single-line human-readable representation
 *
 * Negative duration means the experiment did not terminate.
 *
 * Objectives of this class are not copyable.
 *
 * Access to members is thread-safe.
 */
template <class IN, class OUT>
class ExperimentData final
{
  MOVEONLY(ExperimentData);
  using Data = std::deque<std::tuple<IN, OUT, double>>;

 public:
  class Raii final
  {
   public:
    Raii(ExperimentData& aParent, IN&& aIn)
        : theParent(aParent)
        , theIn(std::forward<IN>(aIn))
        , theChrono(true)
        , theDuration(-1) {
      // noop
    }

    const IN& in() noexcept {
      return theIn;
    }

    void finish(OUT&& aOut) {
      theDuration = theChrono.stop();
      theOut      = std::move(aOut);
    }

    ~Raii() {
      LOG_IF(WARNING, theChrono)
          << "experiment started but not terminated: " << theIn.toString();
      theParent.push(std::move(theIn), std::move(theOut), theDuration);
    }

   private:
    ExperimentData& theParent;
    IN              theIn;
    OUT             theOut;
    support::Chrono theChrono;
    double          theDuration;
  };

  ExperimentData()
      : theData()
      , theMutex() {
    // noop
  }

  /**
   * @brief Return the number of experiments
   *
   * @return std::size_t
   */
  std::size_t size() const noexcept {
    const std::lock_guard<std::mutex> myLock(theMutex);
    return theData.size();
  }

  /**
   * @brief Save the content in memory to the output stream.
   *
   * @param aStream The stream where to save the data, one experiment per line.
   */
  void toCsv(std::ostream& aStream) const {
    const std::lock_guard<std::mutex> myLock(theMutex);
    for (const auto& elem : theData) {
      aStream << std::get<0>(elem).toCsv() << ',' << std::get<1>(elem).toCsv()
              << ',' << std::get<2>(elem) << '\n';
    }
    aStream << std::flush;
  }

  /**
   * @brief Dump the last experiment parameters in a human-readable manner.
   */
  std::string lastIn() const {
    const std::lock_guard<std::mutex> myLock(theMutex);
    if (theData.empty()) {
      throw std::runtime_error("no experiment data");
    }
    return std::get<0>(theData.back()).toString();
  }

  /**
   * @brief Dump the last experiment output in a human-readable manner.
   */
  std::string lastOut() const {
    const std::lock_guard<std::mutex> myLock(theMutex);
    if (theData.empty()) {
      throw std::runtime_error("no experiment data");
    }
    return std::get<1>(theData.back()).toString();
  }

  /**
   * @brief Peek the last experiment output.
   *
   * @throw std::runtime_error if there are no experiments.
   */
  const OUT& peek() const {
    const std::lock_guard<std::mutex> myLock(theMutex);
    if (theData.empty()) {
      throw std::runtime_error("no experiment data");
    }
    return std::get<1>(theData.back());
  }

 private:
  /**
   * @brief Add a new experiment.
   *
   * @param aIn The parameters of the experiment.
   * @param aOut The output of the experiment.
   * @param aDuration The experiment duration.
   */
  void push(IN&& aIn, OUT&& aOut, const double aDuration) {
    const std::lock_guard<std::mutex> myLock(theMutex);
    theData.emplace_back(aIn, aOut, aDuration);
  }

 private:
  Data               theData;
  mutable std::mutex theMutex;
};

} // namespace support
} // namespace uiiit
