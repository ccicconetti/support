#include "signalhandlerflag.h"

#include <atomic>
#include <csignal>

namespace uiiit {
namespace support {

std::atomic<bool> theInterruptedFlag{false};

SignalHandlerFlag::SignalHandlerFlag() {
  signal(SIGINT, [](int) { theInterruptedFlag.exchange(true); });
}

bool SignalHandlerFlag::flag() const noexcept {
  return theInterruptedFlag.load();
}

} // namespace support
} // namespace uiiit
