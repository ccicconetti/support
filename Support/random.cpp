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

#include "random.h"

#include <chrono>

namespace uiiit {
namespace support {

float random() {
  static std::default_random_engine myGenerator(
      std::chrono::system_clock::now().time_since_epoch().count());
  static std::uniform_real_distribution<float> myDistribution(0.0f, 1.0f);
  return myDistribution(myGenerator);
}

GenericRv::GenericRv(const size_t a, const size_t b, const size_t c)
    : theSeed({a, b, c})
    , theGenerator(theSeed) {
}

ConstantRv::ConstantRv(const double aValue) : theValue(aValue) {}

  double ConstantRv::operator()() {
    return theValue;
  }

UniformRv::UniformRv(const double aMin,
                     const double aMax,
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

double UniformRv::operator()() {
  return theRv(theGenerator);
}

ExponentialRv::ExponentialRv(const double aLambda,
                             const size_t a,
                             const size_t b,
                             const size_t c)
    : GenericRv(a, b, c)
    , theRv(aLambda) {
}

double ExponentialRv::operator()() {
  return theRv(theGenerator);
}

PoissonRv::PoissonRv(const double aMu,
                     const size_t a,
                     const size_t b,
                     const size_t c)
    : GenericRv(a, b, c)
    , theRv(aMu) {
}

size_t PoissonRv::operator()() {
  return theRv(theGenerator);
}

} // namespace support
} // namespace uiiit
