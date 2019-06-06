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

#include "Support/movingvariance.h"

#include "gtest/gtest.h"

namespace uiiit {
namespace support {

struct TestMovingVariance : public ::testing::Test {};

TEST_F(TestMovingVariance, test_ctor) {
  ASSERT_NO_THROW(MovingVariance(2));
}

TEST_F(TestMovingVariance, test_exceptions) {
  ASSERT_THROW(MovingVariance(0), ZeroWindow);
  ASSERT_THROW(MovingVariance(1), ZeroWindow);
}

TEST_F(TestMovingVariance, test_variance) {
  MovingVariance myWnd(10);

  ASSERT_TRUE(myWnd.empty());
  ASSERT_FLOAT_EQ(0.0, myWnd.variance());

  myWnd.add(0);
  ASSERT_FALSE(myWnd.empty());
  ASSERT_FLOAT_EQ(0.0, myWnd.variance());

  for (double i = 1; i <= 9; i++) {
    myWnd.add(i);
  }
  ASSERT_FLOAT_EQ(8.25, myWnd.variance());

  myWnd.add(100);
  ASSERT_FLOAT_EQ(818.25, myWnd.variance());

  myWnd.add(200);
  ASSERT_FLOAT_EQ(3845.04, myWnd.variance());

  for (double i = 0; i <= 9; i++) {
    myWnd.add(i);
  }
  ASSERT_FLOAT_EQ(8.25, myWnd.variance());
}

} // namespace support
} // namespace uiiit
