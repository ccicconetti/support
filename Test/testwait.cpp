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

#include "gtest/gtest.h"

#include "Support/chrono.h"
#include "Support/testutils.h"
#include "Support/wait.h"

namespace uiiit {
namespace support {

struct TestWait : public ::testing::Test {};

TEST_F(TestWait, test_waitfor) {
  ASSERT_TRUE(support::waitFor<int>([]() { return 7; }, 7, 0.1));

  Chrono myChrono(true);
  ASSERT_FALSE(support::waitFor<int>([]() { return 8; }, 7, 0.1));
  const auto myElapsed = myChrono.stop();
  ASSERT_GT(myElapsed, 0.1);

  int myCounter = 0;
  ASSERT_TRUE((
      support::waitFor<int>([&myCounter]() { return myCounter++; }, 100, 1.0)));

  WAIT_FOR([]() { return true; }, 0.1) << "should not happen";
}

} // namespace support
} // namespace uiiit
