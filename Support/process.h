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
