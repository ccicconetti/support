#pragma once

#include <cmath>

namespace uiiit {
namespace support {
namespace math {

template <class T>
static T factorial(const T aValue) {
  T ret = 1;
  for (T i = 2; i <= aValue; i++) {
    ret *= i;
  }
  return ret;
}

template <class T>
double erlangC(const unsigned int c, const T rho) {
  T mySum{0};
  for (unsigned int i = 0; i < c; i++) {
    mySum += pow(c * rho, i) / factorial(i);
  }
  return 1.0 / (1.0 + (1.0 - rho) * (factorial(c) / pow(c * rho, c)) * mySum);
}

} // namespace math
} // namespace support
} // namespace uiiit
