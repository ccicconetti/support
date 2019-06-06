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

#include "periodictask.h"

#include <glog/logging.h>

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace uiiit {
namespace support {

PeriodicTask::PeriodicTask(const Task& aTask, const double aPeriod)
    : theTask(aTask)
    , theSleepTime(static_cast<int64_t>(round(aPeriod * 1e9)))
    , theMutex()
    , theCondition()
    , theThread()
    , theTerminating(false) {
  if (not aTask) {
    throw std::runtime_error("The task is not callable");
  }
  if (aPeriod < 0) {
    throw std::runtime_error("Invalid negative period: " +
                             std::to_string(aPeriod));
  }
  theThread.reset(new std::thread(
      [this]() {
    // loop until the destructor is called
    while (true) {
      std::unique_lock<std::mutex> myLock(theMutex);
      theCondition.wait_for(
          myLock, theSleepTime, [this]() { return theTerminating; });
      if (theTerminating) {
        break;
      }
      myLock.unlock();

      // execute the task
      try {
        theTask();
      } catch (const std::exception& aErr) {
        LOG(ERROR) << "Exception caught in periodic task: " << aErr.what();
      } catch (...) {
        LOG(ERROR) << "Unknown exception caught in periodic task";
      }
    }}));
}

PeriodicTask::~PeriodicTask() {
    assert(theThread);
    {
      const std::lock_guard<std::mutex> myLock(theMutex);
      theTerminating = true;
      theCondition.notify_one();
    }
    theThread->join();
}

} // namespace support
} // namespace support
