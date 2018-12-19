#include "signalhandlerwait.h"

#include <condition_variable>
#include <csignal>
#include <mutex>

namespace uiiit {
namespace support {

bool                    theInterrupted{false};
std::mutex              theInterruptedMutex;
std::condition_variable theInterruptedCv;

SignalHandlerWait::SignalHandlerWait() {
  signal(SIGINT, [](int) {
    const std::lock_guard<std::mutex> myLock(theInterruptedMutex);
    theInterrupted = true;
    theInterruptedCv.notify_one();
  });
}

void SignalHandlerWait::wait() const {
  std::unique_lock<std::mutex> myLock(theInterruptedMutex);
  theInterruptedCv.wait(myLock, []() { return theInterrupted; });
}

} // namespace support
} // namespace uiiit
