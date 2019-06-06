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

#include "Support/movingwnd.h"
#include "Support/testutils.h"

#include "gtest/gtest.h"

#include <chrono>
#include <thread>

namespace uiiit {
namespace support {

struct TestMovingWnd : public ::testing::Test {};

TEST_F(TestMovingWnd, test_ctor) { ASSERT_NO_THROW(MovingWnd<int>(1)); }

TEST_F(TestMovingWnd, test_exceptions) {
  ASSERT_THROW(MovingWnd<int>(0), ZeroWindow);
}

TEST_F(TestMovingWnd, test_int) {
  MovingWnd<int> myWnd(5);

  ASSERT_EQ(std::vector<int>({}), myWnd.values());
  ASSERT_TRUE(myWnd.empty());
  ASSERT_FALSE(myWnd.full());
  ASSERT_EQ(0u, myWnd.size());

  myWnd.add(7);
  ASSERT_EQ(std::vector<int>({7}), myWnd.values());
  ASSERT_FALSE(myWnd.empty());
  ASSERT_EQ(1u, myWnd.size());

  for (int i = 0; i < 5; i++) {
    myWnd.add(i);
  }
  ASSERT_EQ(std::vector<int>({0, 1, 2, 3, 4}), myWnd.values());
  ASSERT_FALSE(myWnd.empty());
  ASSERT_TRUE(myWnd.full());
  ASSERT_EQ(5u, myWnd.size());

  myWnd.add(7);
  ASSERT_EQ(std::vector<int>({1, 2, 3, 4, 7}), myWnd.values());
  ASSERT_TRUE(myWnd.full());
  ASSERT_FALSE(myWnd.empty());
  ASSERT_EQ(5u, myWnd.size());

  myWnd.clear();
  ASSERT_EQ(std::vector<int>({}), myWnd.values());
  ASSERT_TRUE(myWnd.empty());
  ASSERT_FALSE(myWnd.full());
  ASSERT_EQ(0u, myWnd.size());

  myWnd.add(-1);
  myWnd.add(0);
  myWnd.add(1);
  ASSERT_EQ(std::vector<int>({-1, 0, 1}), myWnd.values());
  ASSERT_FALSE(myWnd.empty());
  ASSERT_EQ(3u, myWnd.size());

  ASSERT_EQ(-1, myWnd.pop());
  ASSERT_EQ(0, myWnd.pop());
  ASSERT_EQ(1, myWnd.pop());
  ASSERT_THROW(myWnd.pop(), EmptyWindow);
}

TEST_F(TestMovingWnd, test_purge) {
  MovingWnd<int> myWnd(5);
  for (int i = 0; i < 4; i++) {
    myWnd.add(i);
  }
  ASSERT_EQ(std::vector<int>({0, 1, 2, 3}), myWnd.values());
  ASSERT_FALSE(myWnd.purge(1));
  ASSERT_EQ(std::vector<int>({0, 1, 2, 3}), myWnd.values());

  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  myWnd.add(4);
  ASSERT_EQ(std::vector<int>({0, 1, 2, 3, 4}), myWnd.values());

  WAIT_FOR(
      [&myWnd]() {
        myWnd.purge(0.1);
        return myWnd.values().size() == 1;
      },
      0.15)
      << myWnd.values().size();
  ASSERT_EQ(std::vector<int>({4}), myWnd.values());

  WAIT_FOR([&myWnd]() { return myWnd.purge(0.1); }, 0.15);
  ASSERT_EQ(std::vector<int>({}), myWnd.values());
  ASSERT_TRUE(myWnd.empty());
}

} // namespace support
} // namespace uiiit
