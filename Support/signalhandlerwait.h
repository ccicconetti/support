#pragma once

#include "Support/macros.h"

namespace uiiit {
namespace support {

/**
 * Wait until SIGINT is caught.
 */
class SignalHandlerWait final
{
  NONCOPYABLE_NONMOVABLE(SignalHandlerWait);

 public:
  SignalHandlerWait();

  //! Expected to be called just once in a program's lifetime.
  void wait() const;
};

} // namespace support
} // namespace uiiit
