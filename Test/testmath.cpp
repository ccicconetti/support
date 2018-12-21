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

#include "Support/math.h"

#include "gtest/gtest.h"

namespace uiiit {
namespace support {
namespace math {

struct TestMath : public ::testing::Test {};

TEST_F(TestMath, test_factorial) {
  ASSERT_EQ(1, factorial(-1));
  ASSERT_EQ(1, factorial(0));
  ASSERT_EQ(1, factorial(1));
  ASSERT_EQ(2, factorial(2));
  ASSERT_EQ(120, factorial(5));
  ASSERT_FLOAT_EQ(6.0, factorial(3.14));
  ASSERT_FLOAT_EQ(120.0, factorial(5.0));
}

TEST_F(TestMath, test_erlangc) {
  ASSERT_FLOAT_EQ(1.0, erlangC(0, 1.0));
  ASSERT_FLOAT_EQ(0.33333333, erlangC(2, 0.5));
}

} // namespace math
} // namespace support
} // namespace uiiit
