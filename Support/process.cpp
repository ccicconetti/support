#include "process.h"

#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <stdexcept>

#ifndef RUSAGE_SELF
#define RUSAGE_SELF 0
#endif

namespace uiiit {
namespace support {

ProcessLoad::ProcessLoad()
    : theChrono(false)
    , theRusage()
    , theLastUsrTime(0)
    , theLastSysTime(0) {
  if (getrusage(RUSAGE_SELF, &theRusage) != 0) {
    throw std::runtime_error(strerror(errno));
  }
  theLastUsrTime = convertTimeval(theRusage.ru_utime);
  theLastSysTime = convertTimeval(theRusage.ru_stime);
  theChrono.start();
}

std::pair<double, double> ProcessLoad::operator()() {
  std::pair<double, double> ret({0, 0});

  if (getrusage(RUSAGE_SELF, &theRusage) != 0) {
    throw std::runtime_error(strerror(errno));
  }

  const auto myUsrTime = convertTimeval(theRusage.ru_utime);
  const auto mySysTime = convertTimeval(theRusage.ru_stime);
  const auto myElapsed = theChrono.restart();

  assert(myUsrTime >= theLastUsrTime);
  assert(mySysTime >= theLastSysTime);
  assert(myElapsed != 0);

  ret.first  = (myUsrTime - theLastUsrTime) / myElapsed;
  ret.second = (mySysTime - theLastSysTime) / myElapsed;

  theLastUsrTime = myUsrTime;
  theLastSysTime = mySysTime;

  return ret;
}

double ProcessLoad::convertTimeval(const timeval& aTimeVal) {
  return static_cast<double>(aTimeVal.tv_sec) +
         1e-6 * static_cast<double>(aTimeVal.tv_usec);
}

} // namespace support
} // namespace uiiit
