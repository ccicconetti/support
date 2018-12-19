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
