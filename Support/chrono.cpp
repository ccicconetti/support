#include "chrono.h"

namespace uiiit {
namespace support {

Chrono::Chrono(const bool aAutostart)
    : theStarted(aAutostart)
    , theStart(std::chrono::high_resolution_clock::now()) {
}

void Chrono::start() {
  theStart   = std::chrono::high_resolution_clock::now();
  theStarted = true;
}

double Chrono::stop() {
  const auto myElapsed = time();
  theStarted           = false;
  return myElapsed;
}

double Chrono::restart() {
  double     myRet = 0;
  const auto myNow = std::chrono::high_resolution_clock::now();
  if (theStarted) {
    myRet = std::chrono::duration_cast<std::chrono::duration<double>>(myNow -
                                                                      theStart)
                .count();
  } else {
    theStarted = true;
  }
  theStart = myNow;
  return myRet;
}

double Chrono::time() const {
  if (not theStarted) {
    return 0;
  }

  const auto myNow = std::chrono::high_resolution_clock::now();
  const auto myElapsed =
      std::chrono::duration_cast<std::chrono::duration<double>>(myNow -
                                                                theStart);
  return myElapsed.count();
}

} // namespace support
} // namespace uiiit
