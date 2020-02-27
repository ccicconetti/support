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

#include "Support/process.h"

#include <chrono>
#include <cmath>
#include <thread>

#include "gtest/gtest.h"

namespace uiiit {
namespace support {

struct TestProcess : public ::testing::Test {};

TEST_F(TestProcess, test_ctor) {
  ProcessLoad myLoad;
}

TEST_F(TestProcess, test_query_empty) {
  ProcessLoad myLoad;

  // do nothing
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  const auto myLoad1 = myLoad();
  EXPECT_NEAR(0.0, myLoad1.first, 0.1);
  EXPECT_NEAR(0.0, myLoad1.second, 0.1);

  // do something
  for (double i = 1; i < 10000000; i++) {
    std::ignore = log10(sin(i * i * i));
  }

  const auto myLoad2 = myLoad();
  EXPECT_GE(myLoad2.first, 0.9);
  EXPECT_NEAR(0.0, myLoad2.second, 0.1);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  // call with a 1 ms pause in between
  for (auto i = 0; i < 100; i++) {
    const auto myCurLoad = myLoad();
    EXPECT_GE(myCurLoad.first, 0.0);
    EXPECT_LE(myCurLoad.first, 1.0);
    EXPECT_GE(myCurLoad.second, 0.0);
    EXPECT_LE(myCurLoad.second, 1.0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

} // namespace support
} // namespace uiiit
