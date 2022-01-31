/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2022 Claudio Cicconetti https://ccicconetti.github.io/

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

#include "Support/parallelbatch.h"
#include "Support/queue.h"

#include "gtest/gtest.h"

#include <chrono>
#include <cstdlib>
#include <mutex>
#include <stdexcept>
#include <thread>

namespace uiiit {
namespace support {

struct TestParallelBatch : public ::testing::Test {
  TestParallelBatch() {
    srand48(time(nullptr));
  }
};

TEST_F(TestParallelBatch, test_no_exception) {
  for (const auto& myCallWait : std::list<bool>({true, false})) {
    if (myCallWait) {
      LOG(INFO) << "explicit wait for the experiment termination";
    } else {
      LOG(INFO) << "do not wait explicitly for the experiment termination";
    }

    Queue<int> myInputs;
    for (auto i = 0; i < 20; i++) {
      myInputs.push(i);
    }
    std::mutex         myMutex;
    std::set<int>      myDone;
    ParallelBatch<int> myParallelBatch(
        5, myInputs, [&myMutex, &myDone](int&& x) {
          VLOG(1) << "executing experiment #" << x;
          std::this_thread::sleep_for(
              std::chrono::milliseconds(lrand48() % 100));

          const std::lock_guard<std::mutex> myLock(myMutex);
          myDone.insert(x);
        });

    if (myCallWait) {
      const auto myExceptions = myParallelBatch.wait();
      ASSERT_TRUE(myExceptions.empty());
      ASSERT_EQ(20, myDone.size());
    }
  }
}

TEST_F(TestParallelBatch, test_with_exceptions) {
  Queue<int> myInputs;
  for (auto i = 0; i < 20; i++) {
    myInputs.push(i);
  }
  std::mutex         myMutex;
  std::set<int>      myDone;
  ParallelBatch<int> myParallelBatch(5, myInputs, [&myMutex, &myDone](int&& x) {
    VLOG(1) << "executing experiment #" << x;
    if (x % 2 == 1) {
      std::this_thread::sleep_for(std::chrono::milliseconds(lrand48() % 100));
    } else {
      throw std::runtime_error("exception");
    }

    const std::lock_guard<std::mutex> myLock(myMutex);
    myDone.insert(x);
  });

  const auto myExceptions = myParallelBatch.wait();
  ASSERT_EQ(10, myExceptions.size());
  ASSERT_EQ(10, myDone.size());
}

} // namespace support
} // namespace uiiit
