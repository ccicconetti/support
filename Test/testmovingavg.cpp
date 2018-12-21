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

#include "Support/movingavg.h"

#include "gtest/gtest.h"

namespace uiiit {
namespace support {

struct TestMovingAvg : public ::testing::Test {};

TEST_F(TestMovingAvg, test_ctor) {
  ASSERT_NO_THROW(MovingAvg<int>(1));
}

TEST_F(TestMovingAvg, test_exceptions) {
  ASSERT_THROW(MovingAvg<int>(0), ZeroWindow);

  MovingAvg<int> myWnd(1);
  ASSERT_TRUE(myWnd.empty());
  ASSERT_THROW(myWnd.average(), EmptyWindow);
  ASSERT_THROW(myWnd.last(), EmptyWindow);

  myWnd.add(0);
  ASSERT_FALSE(myWnd.empty());
  ASSERT_NO_THROW(myWnd.average());
  ASSERT_NO_THROW(myWnd.last());

  myWnd.clear();
  ASSERT_TRUE(myWnd.empty());
  ASSERT_THROW(myWnd.average(), EmptyWindow);
  ASSERT_THROW(myWnd.last(), EmptyWindow);
}

TEST_F(TestMovingAvg, test_uint) {
  MovingAvg<unsigned int> myWnd(5);
  myWnd.add(1);
  ASSERT_EQ(1u, myWnd.average());
  ASSERT_EQ(1u, myWnd.last());
  myWnd.add(2);
  ASSERT_EQ(1u, myWnd.average());
  ASSERT_EQ(2u, myWnd.last());
  myWnd.add(3);
  ASSERT_EQ(2u, myWnd.average());
  myWnd.add(4);
  ASSERT_EQ(2u, myWnd.average());
  myWnd.add(5);
  ASSERT_EQ(3u, myWnd.average());
  myWnd.add(6);
  ASSERT_EQ(4u, myWnd.average());
  myWnd.add(7);
  myWnd.last();
  ASSERT_EQ(5u, myWnd.average());

  myWnd.clear();
  myWnd.add(8);
  ASSERT_EQ(8u, myWnd.average());

  for (auto i = 0; i < 100; i++) {
    myWnd.add(i);
    ASSERT_EQ(i, myWnd.last()) << i;
  }
  for (auto i = 0; i < 5; i++) {
    myWnd.add(42);
    ASSERT_EQ(42u, myWnd.last()) << i;
  }
  ASSERT_EQ(42u, myWnd.average());
}

TEST_F(TestMovingAvg, test_float) {
  MovingAvg<float> myWnd(5);
  myWnd.add(1);
  ASSERT_FLOAT_EQ(1.0f, myWnd.average());
  myWnd.add(2);
  ASSERT_FLOAT_EQ(1.5f, myWnd.average());
  myWnd.add(3);
  ASSERT_FLOAT_EQ(2.0f, myWnd.average());
  myWnd.add(4);
  ASSERT_FLOAT_EQ(2.5f, myWnd.average());
  myWnd.add(5);
  ASSERT_FLOAT_EQ(3.0f, myWnd.average());
  myWnd.add(6);
  ASSERT_FLOAT_EQ(4.0f, myWnd.average());
  myWnd.add(7);
  ASSERT_FLOAT_EQ(5.0f, myWnd.average());

  myWnd.clear();
  myWnd.add(8);
  ASSERT_FLOAT_EQ(8.0f, myWnd.average());
}

} // namespace support
} // namespace uiiit
