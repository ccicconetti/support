#pragma once

#include <chrono>

namespace uiiit {
namespace support {

class Chrono final
{
 public:
  explicit Chrono(const bool aAutostart);

  //! Start a timer. If already started, reset to the current time.
  void start();

  //! Stop the timer.
  double stop();

  //! Restart the tiner, or start if not started.
  //! \return the fractional seconds since the start time, or 0 if not started.
  double restart();

  //! \return the fraction seconds since the start time, or 0 if not started.
  double time() const;

  //! \return true if the timer is started.
  operator bool() const noexcept {
    return theStarted;
  }

 private:
  bool                                           theStarted;
  std::chrono::high_resolution_clock::time_point theStart;
};

} // namespace support
} // namespace uiiit
