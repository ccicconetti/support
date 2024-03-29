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

#include "macros.h"

#include <memory>

namespace uiiit {
namespace support {

struct Accumulator;
struct WeightedAccumulator;

class SummaryStat final
{
 public:
  MOVEONLY(SummaryStat);

  using Real = double;

 public:
  explicit SummaryStat();
  ~SummaryStat() = default;

  //! Add a new value.
  void operator()(const Real aValue);

  //! Reset this object to its initial state.
  void reset();

  //! \return the mean of the values added, or NaN if there are none.
  Real mean() const;

  //! \return the minimum value added.
  Real min() const;

  //! \return the maximum value added.
  Real max() const;

  //! \return the standard deviation of the values added, or NaN if there are
  //! less than two values.
  Real stddev() const;

  //! \return true if no values were added.
  bool empty() const;

  //! \return the number of values added.
  size_t count() const;

 private:
  std::shared_ptr<Accumulator> theAcc; // could be unique_ptr
};

class SummaryWeightedStat
{
 public:
  /**
   * @brief Construct a new weighted accumulator object and link it to a given
   * clock.
   *
   * @param aClock the external clock, which exists until this object is alive
   * @param aWarmUp the initial warm-up period in which samples are discarded
   */
  explicit SummaryWeightedStat(const double& aClock, const double aWarmUp);
  ~SummaryWeightedStat() = default;

  //! add a new value with weight given by the clock
  void operator()(const double aValue);

  //! @return the weighted mean
  double mean();

  //! @return the number of samples added
  size_t count() const;

 private:
  const double&                        theClock;
  const double                         theWarmUp;
  double                               theLastClock;
  double                               theLastValue;
  bool                                 theInitialized;
  std::shared_ptr<WeightedAccumulator> theAcc; // could be unique_ptr
};

} // namespace support
} // namespace uiiit
