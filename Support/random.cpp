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
