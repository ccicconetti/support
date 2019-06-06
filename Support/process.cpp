/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2019 Claudio Cicconetti https://ccicconetti.github.io/

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
