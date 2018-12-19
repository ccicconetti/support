#pragma once

#include "Support/macros.h"

namespace uiiit {
namespace support {

/**
 * Captures the SIGINT and set a flag when this signal is received.
 *
 * The expected usage is to create a single instance of this class at the
 * beginning of main(), then use the flag() method to check if the SIGINT signal
 * has been received and then terminate. Further interrupts by the same signal
 * do not have effect on the return value of flag().
 */
class SignalHandlerFlag final
{
  NONCOPYABLE_NONMOVABLE(SignalHandlerFlag);

 public:
  SignalHandlerFlag();

  bool flag() const noexcept;
};

} // namespace support
} // namespace uiiit
