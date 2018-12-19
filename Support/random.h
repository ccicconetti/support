#pragma once

#include "Support/macros.h"

#include <random>
#include <stdexcept>

namespace uiiit {
namespace support {

//! \return a random number in [0, 1].
float random();

//! Generic r.v.
class GenericRv
{
  NONCOPYABLE_NONMOVABLE(GenericRv);

 protected:
  explicit GenericRv(const size_t a, const size_t b, const size_t c);

 protected:
  std::seed_seq theSeed;
  std::mt19937  theGenerator;
};

template <class TYPE>
class UniformIntRv : public GenericRv
{
 public:
  explicit UniformIntRv(const TYPE   aMin,
                        const TYPE   aMax,
                        const size_t a,
                        const size_t b,
                        const size_t c);

  TYPE operator()();

 private:
  std::uniform_int_distribution<TYPE> theRv;
};

class UniformRv : public GenericRv
{
 public:
  explicit UniformRv(const double aMin,
                     const double aMax,
                     const size_t a,
                     const size_t b,
                     const size_t c);

  double operator()();

 private:
  std::uniform_real_distribution<double> theRv;
};

class ExponentialRv : public GenericRv
{
 public:
  explicit ExponentialRv(const double aLambda,
                         const size_t a,
                         const size_t b,
                         const size_t c);

  double operator()();

 private:
  std::exponential_distribution<double> theRv;
};

class PoissonRv : public GenericRv
{
 public:
  explicit PoissonRv(const double aMu,
                     const size_t a,
                     const size_t b,
                     const size_t c);

  size_t operator()();

 private:
  std::poisson_distribution<size_t> theRv;
};

////////////////////////////////////////////////////////////////////////////////

template <class TYPE>
UniformIntRv<TYPE>::UniformIntRv(const TYPE   aMin,
                                 const TYPE   aMax,
                                 const size_t a,
                                 const size_t b,
                                 const size_t c)
    : GenericRv(a, b, c)
    , theRv(aMin, aMax) {
  if (aMin > aMax) {
    throw std::runtime_error("Invalid range for UniformRv: [" +
                             std::to_string(aMin) + ":" + std::to_string(aMax) +
                             "]");
  }
}

template <class TYPE>
TYPE UniformIntRv<TYPE>::operator()() {
  return theRv(theGenerator);
}

} // namespace support
} // namespace uiiit
