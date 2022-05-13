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

#include "Support/macros.h"

#include <cassert>
#include <numeric>
#include <random>
#include <set>
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

/**
 * @brief Pick an element from a container
 *
 * @tparam CONTAINER The type of the container.
 * @param aContainer The source container.
 * @param aRv A r.v. that generates number in [0,1].
 * @return CONTAINER::value_type The element chosen.
 */
template <typename CONTAINER>
typename CONTAINER::value_type choice(const CONTAINER& aContainer,
                                      RealRvInterface& aRv) {
  if (aContainer.empty()) {
    throw std::runtime_error(
        "cannot select an element from an empty container");
  }
  auto myOffset = aContainer.size();
  while (myOffset >= aContainer.size()) {
    const auto myRnd = aRv();
    assert(myRnd >= 0 and myRnd <= 1);
    myOffset =
        static_cast<typename CONTAINER::size_type>(myRnd * aContainer.size());
  }
  assert(myOffset >= 0 and myOffset < aContainer.size());
  auto it = aContainer.begin();
  std::advance(it, myOffset);
  assert(it != aContainer.end());
  return *it;
}

/**
 * @brief Return a sample of k (unique) elements from a container.
 *
 * @tparam CONTAINER The type of the container.
 * @param aContainer The source container.
 * @param aK The number of elements sampled.
 * @param aRv A r.v. that generates number in [0,1].
 * @return CONTAINER The elements sampled.
 */
template <typename CONTAINER>
CONTAINER sample(const CONTAINER&  aContainer,
                 const std::size_t aK,
                 RealRvInterface&  aRv) {
  CONTAINER ret;
  if (aContainer.size() <= aK) {
    std::copy(
        aContainer.begin(), aContainer.end(), std::inserter(ret, ret.begin()));

  } else {
    std::set<typename CONTAINER::value_type> myFound;
    for (std::size_t i = 0; i < aK; i++) {
      while (myFound.emplace(choice(aContainer, aRv)).second == false) {
        // noop
      }
    }
    for (auto it = myFound.begin(); it != myFound.end(); it = myFound.begin()) {
      ret.insert(ret.begin(), std::move(myFound.extract(it).value()));
    }
  }

  return ret;
}

/**
 * @brief Return a sample of k (unique) elements from a container, with the
 * probability of any given element proportional to its corresponding weight.
 *
 * @tparam CONTAINER The type of the container.
 @ @tparam CONTAINER_WEIGHTS The type of the weights' container.
 * @param aContainer The source container.
 * @param aWeights The weights.
 * @param aK The number of elements sampled.
 * @param aRv A r.v. that generates number in [0,1].
 * @return CONTAINER The elements sampled.
 */
template <typename CONTAINER, typename CONTAINER_WEIGHTS>
CONTAINER sampleWeighted(const CONTAINER&         aContainer,
                         const CONTAINER_WEIGHTS& aWeights,
                         const std::size_t        aK,
                         RealRvInterface&         aRv) {
  if (aContainer.size() != aWeights.size()) {
    throw std::runtime_error("container and weights must have the same size: " +
                             std::to_string(aContainer.size()) + " vs " +
                             std::to_string(aWeights.size()));
  }

  CONTAINER ret;
  if (aContainer.size() <= aK) {
    std::copy(
        aContainer.begin(), aContainer.end(), std::inserter(ret, ret.begin()));

  } else {
    double mySum = std::accumulate(aWeights.begin(), aWeights.end(), 0.0);
    std::set<typename CONTAINER::const_iterator> myFound;
    for (std::size_t i = 0; i < aK; i++) {
      const auto myRnd = aRv() * mySum;
      assert(myRnd >= 0 and myRnd <= mySum);
      auto it    = aContainer.begin();
      auto jt    = aWeights.begin();
      auto myCur = 0.0;
      for (; it != aContainer.end(); ++it, ++jt) {
        assert(jt != aWeights.end());
        if (myFound.find(it) != myFound.end()) {
          // skip the weight of elements already found
          continue;
        }
        myCur += *jt;
        if (myRnd <= myCur) {
          myFound.emplace(it);
          mySum -= *jt;
          break;
        }
      }
      assert(it != aContainer.end());
    }
    assert(myFound.size() == aK);
    while (not myFound.empty()) {
      ret.insert(ret.begin(), *myFound.extract(myFound.begin()).value());
    }
  }

  return ret;
}

template <typename CONTAINER>
class SetRv : public GenericRv, public RealRvInterface
{
 public:
  explicit SetRv(const CONTAINER& aValues,
                 const size_t     a,
                 const size_t     b,
                 const size_t     c)
      : GenericRv(a, b, c)
      , theValues(aValues)
      , theUniformRv(0, 1, a, b, c) {
    // noop
  }

  double operator()() override {
    return choice(theValues, theUniformRv);
  }

 private:
  CONTAINER theValues;
  UniformRv theUniformRv;
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
