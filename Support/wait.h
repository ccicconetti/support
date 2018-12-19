#pragma once

#include <chrono>
#include <functional>
#include <thread>

namespace uiiit {
namespace support {

template <class T>
bool waitFor(const std::function<T(void)>& aPred,
             const T&                      aExpected,
             const double                  aTimeout) {
  const auto myStartTime = std::chrono::high_resolution_clock::now();
  while (std::chrono::duration_cast<std::chrono::duration<double>>(
             std::chrono::high_resolution_clock::now() - myStartTime)
             .count() < aTimeout) {
    if (aPred() == aExpected) {
      return true;
    }
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
  return false;
}

} // namespace support
} // namespace uiiit
