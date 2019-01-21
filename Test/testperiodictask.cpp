/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2018 Claudio Cicconetti <https://about.me/ccicconetti>

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

#include "Support/chrono.h"
#include "Support/macros.h"
#include "Support/periodictask.h"
#include "Support/wait.h"

#include "gtest/gtest.h"

#include <atomic>

namespace uiiit {
namespace support {

struct TestPeriodicTask : public ::testing::Test {};

TEST_F(TestPeriodicTask, test_invalid) {
  PeriodicTask::Task myCallable([]() {});
  ASSERT_THROW(PeriodicTask(myCallable, -1.0), std::runtime_error);

  PeriodicTask::Task myNotCallable;
  ASSERT_THROW(PeriodicTask(myNotCallable, 1.0), std::runtime_error);

  ASSERT_NO_THROW(PeriodicTask(myCallable, 1.0));
}

TEST_F(TestPeriodicTask, test_execution) {
  std::atomic<int>   myCounter{0};
  PeriodicTask::Task myCallable([&]() { myCounter++; });
  PeriodicTask       myTask(myCallable, 0.1);
  Chrono             myChrono(true);
  ASSERT_TRUE(waitFor<int>([&]() { return myCounter.load(); }, 10, 1.2));
  ASSERT_GE(myChrono.stop(), 0.9);
}

TEST_F(TestPeriodicTask, test_exception) {
  auto myCaught = false;
  try {
    PeriodicTask::Task myCallable(
        [&]() { throw std::runtime_error("Epic fail"); });
    PeriodicTask myTask(myCallable, 0.1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  } catch (...) {
    myCaught = true;
  }
  ASSERT_FALSE(myCaught);
}

} // namespace support
} // namespace uiiit
