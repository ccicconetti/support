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

#include <memory>

namespace uiiit {
namespace support {

class Accumulator;

class SummaryStat final
{
 public:
  using Real = double;

 public:
  explicit SummaryStat();
  ~SummaryStat();

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
  std::unique_ptr<Accumulator> theAcc;
};

} // namespace support
} // namespace uiiit
