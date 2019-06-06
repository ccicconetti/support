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

#pragma once

#include "Support/macros.h"

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <memory>
#include <thread>

namespace uiiit {
namespace support {

/**
 * Periodically execute a task.
 */
class PeriodicTask final
{
  NONCOPYABLE_NONMOVABLE(PeriodicTask);

 public:
  using Task = std::function<void(void)>;

  /**
   * Any exception throw by the task is caught within this class, a log line is
   * produced.
   *
   * \param aTask The task to be executed periodically.
   * \param aPeriod The period, in fractional seconds.
   *
   * \throw std::runtime_error if aTask is not callable
   * \throw std::runtime_error if aPeriod < 0
   */
  explicit PeriodicTask(const Task& aTask, const double aPeriod);

  ~PeriodicTask();

 private:
  const Task                     theTask;
  const std::chrono::nanoseconds theSleepTime;

  std::mutex              theMutex;
  std::condition_variable theCondition;
  std::unique_ptr<std::thread>             theThread;
  bool                    theTerminating;
};

} // namespace support
} // namespace uiiit
