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

#include "Support/stat.h"

#include "gtest/gtest.h"

#include <cmath>
#include <limits>

namespace uiiit {
namespace support {

struct TestStat : public ::testing::Test {};

TEST_F(TestStat, test_summary_ctor) {
  ASSERT_NO_THROW(SummaryStat());

  SummaryStat myStat;

  EXPECT_EQ(0u, myStat.count());
  EXPECT_TRUE(std::isnan(myStat.mean()));
  EXPECT_EQ(std::numeric_limits<double>::max(), myStat.min());
  EXPECT_EQ(std::numeric_limits<double>::lowest(), myStat.max());
  EXPECT_EQ(0u, myStat.stddev());
}

TEST_F(TestStat, test_summary_operations) {
  SummaryStat myStat;

  EXPECT_TRUE(myStat.empty());

  myStat(3.14);

  EXPECT_FALSE(myStat.empty());
  EXPECT_EQ(1u, myStat.count());
  EXPECT_FLOAT_EQ(3.14, myStat.mean());
  EXPECT_FLOAT_EQ(3.14, myStat.min());
  EXPECT_FLOAT_EQ(3.14, myStat.max());
  EXPECT_FLOAT_EQ(0u, myStat.stddev());

  myStat.reset();

  EXPECT_EQ(0u, myStat.count());
  EXPECT_TRUE(myStat.empty());

  for (int i = -100; i <= 100; i++) {
    myStat(i);
  }

  EXPECT_FALSE(myStat.empty());
  EXPECT_EQ(201u, myStat.count());
  EXPECT_FLOAT_EQ(0, myStat.mean());
  EXPECT_FLOAT_EQ(-100, myStat.min());
  EXPECT_FLOAT_EQ(100, myStat.max());
  EXPECT_FLOAT_EQ(58.022984, myStat.stddev());
}

} // namespace support
} // namespace uiiit
