/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/support/
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

#pragma once

#include <sys/resource.h>

#include "Support/chrono.h"
#include "Support/macros.h"

#include <utility>

namespace uiiit {
namespace support {

/**
 * Query the user/system load of the current process.
 *
 * Objects of this class are not thread-safe.
 */
class ProcessLoad
{
  NONCOPYABLE_NONMOVABLE(ProcessLoad);

 public:
  //! Class initialization.
  explicit ProcessLoad();

  /**
   * \return the user/system load since this method was last called.
   *
   * \throw std::runtime_error if an error occurs.
   */
  std::pair<double, double> operator()();

 private:
  double convertTimeval(const timeval& aTimeVal);

 private:
  support::Chrono theChrono;
  rusage          theRusage;
  double          theLastUsrTime;
  double          theLastSysTime;
};

} // namespace support
} // namespace uiiit
