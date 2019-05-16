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

//! Real r.v. interface.
class RealRvInterface
{
 public:
  virtual ~RealRvInterface() {
  }

  virtual double operator()() = 0;
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

class ConstantRv : public RealRvInterface
{
 public:
  explicit ConstantRv(const double aValue);

  double operator()() override;

 private:
  const double theValue;
};

class UniformRv : public GenericRv, public RealRvInterface
{
 public:
  explicit UniformRv(const double aMin,
                     const double aMax,
                     const size_t a,
                     const size_t b,
                     const size_t c);

  double operator()() override;

 private:
  std::uniform_real_distribution<double> theRv;
};

class ExponentialRv : public GenericRv, public RealRvInterface
{
 public:
  explicit ExponentialRv(const double aLambda,
                         const size_t a,
                         const size_t b,
                         const size_t c);

  double operator()() override;

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
