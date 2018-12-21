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


#include "Support/queue.h"

#include "gtest/gtest.h"

#include <atomic>
#include <set>
#include <thread>

namespace uiiit {
namespace support {

struct TestQueue : public ::testing::Test {};

TEST_F(TestQueue, test_ctor) {
  ASSERT_NO_THROW(Queue<int>());
}

TEST_F(TestQueue, test_push_pop_size) {
  Queue<int> myQueue;
  for (int i = 0; i < 10; i++) {
    myQueue.push(i);
  }
  ASSERT_EQ(10u, myQueue.size());
  for (int i = 0; i < 10; i++) {
    ASSERT_EQ(i, myQueue.pop());
  }
}

TEST_F(TestQueue, test_multiple_threads) {
  Queue<int>          myQueue;
  std::set<int>       mySet1;
  std::set<int>       mySet2;
  bool                theClosed1 = false;
  bool                theClosed2 = false;
  std::atomic<size_t> myPoppedElements{0};

  std::thread myPusher([&]() {
    for (int i = 0; i < 1000; i++) {
      myQueue.push(i);
    }
    while (myPoppedElements.load() < 1000) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    myQueue.close();
  });
  std::thread myPopper1([&]() {
    try {
      while (true) {
        mySet1.insert(myQueue.pop());
        myPoppedElements++;
      }
    } catch (const QueueClosed&) {
      theClosed1 = true;
    }
  });
  std::thread myPopper2([&]() {
    try {
      while (true) {
        mySet2.insert(myQueue.pop());
        myPoppedElements++;
      }
    } catch (const QueueClosed&) {
      theClosed2 = true;
    }
  });

  myPusher.join();
  myPopper1.join();
  myPopper2.join();

  ASSERT_TRUE(theClosed1);
  ASSERT_TRUE(theClosed2);

  for (int i = 0; i < 1000; i++) {
    ASSERT_TRUE((mySet1.count(i) == 1) xor (mySet2.count(i) == 1));
  }
}

TEST_F(TestQueue, test_not_copyable) {
  struct NotCopyable {
    explicit NotCopyable(int aValue)
        : theValue(aValue) {
    }
    NotCopyable(NotCopyable&&) = default;
    NotCopyable& operator=(NotCopyable&&) = default;
    NotCopyable(const NotCopyable&)       = delete;
    NotCopyable& operator=(const NotCopyable&) = delete;
    const int    theValue;
  };

  Queue<NotCopyable> myQueue;
  myQueue.push(NotCopyable(42));
  NotCopyable myValue(43);
  myQueue.push(std::move(myValue));

  auto myOut = myQueue.pop();
  ASSERT_EQ(42u, myOut.theValue);
  // myOut = myQueue.pop();
  auto myOut2 = myQueue.pop();
  ASSERT_EQ(43u, myOut2.theValue);
}

} // namespace support
} // namespace uiiit
