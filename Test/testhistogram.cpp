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

#include "Support/histogram.h"

#include "gtest/gtest.h"

#include <iostream>
#include <random>

namespace uiiit {
namespace support {

struct TestHistogram : public ::testing::Test {};

TEST_F(TestHistogram, test_ctor) {
  ASSERT_NO_THROW(Histogram(100, 10, 5, Histogram::KEEP));
  ASSERT_NO_THROW(Histogram(100, 10, 5, Histogram::IGNORE));
  ASSERT_NO_THROW(Histogram(100, 10, 5, Histogram::THROW));

  ASSERT_THROW(Histogram(100, 0, 5, Histogram::KEEP), std::runtime_error);
  ASSERT_THROW(Histogram(100, -10, 5, Histogram::KEEP), std::runtime_error);
  ASSERT_THROW(Histogram(100, 10, 0, Histogram::KEEP), std::runtime_error);
}

TEST_F(TestHistogram, test_underflow_overflow_ignore) {
  Histogram myHisto(100, 10, 5, Histogram::IGNORE);

  ASSERT_NO_THROW(myHisto(0, 1));
  ASSERT_NO_THROW(myHisto(2000, 1));

  ASSERT_THROW(myHisto.underflow(), std::runtime_error);
  ASSERT_THROW(myHisto.overflow(), std::runtime_error);
}

TEST_F(TestHistogram, test_underflow_overflow_throw) {
  Histogram myHisto(100, 10, 5, Histogram::THROW);

  ASSERT_THROW(myHisto(0, 1), std::runtime_error);
  ASSERT_THROW(myHisto(2000, 1), std::runtime_error);

  ASSERT_THROW(myHisto.underflow(), std::runtime_error);
  ASSERT_THROW(myHisto.overflow(), std::runtime_error);
}

TEST_F(TestHistogram, test_operations) {
  const size_t N = 100000;
  Histogram    myHisto(100, 10, 5, Histogram::KEEP);

  std::default_random_engine                      myGenerator;
  std::uniform_real_distribution<Histogram::Real> myUniValue(90, 160);
  std::uniform_real_distribution<Histogram::Real> myUniWeight(-10, 10);

  for (size_t i = 0; i < N; i++) {
    myHisto(myUniValue(myGenerator), myUniWeight(myGenerator));
  }

  size_t myCount = 0;
  for (Histogram::Real x = 105; x < 150; x += 10) {
    myCount += myHisto.stat(x).count();
    EXPECT_NEAR(-10, myHisto.stat(x).min(), 0.01);
    EXPECT_NEAR(0, myHisto.stat(x).mean(), 0.1);
    EXPECT_NEAR(10, myHisto.stat(x).max(), 0.01);
    EXPECT_NEAR(5.7, myHisto.stat(x).stddev(), 0.1);
  }
  myCount += myHisto.underflow().count();
  myCount += myHisto.overflow().count();
  ASSERT_EQ(N, myCount);

  myHisto.print(std::cout);
}

TEST_F(TestHistogram, test_closestmean) {
  Histogram myHisto(100, 10, 5, Histogram::IGNORE);

  ASSERT_THROW(myHisto.closestMean(99, false), std::runtime_error);
  ASSERT_THROW(myHisto.closestMean(150, false), std::runtime_error);

  // put values in the middle bin (#2)
  myHisto(125, 1);
  myHisto(125, 2);
  myHisto(125, 3);

  for (Histogram::Real x = 120; x < 130; x += 0.1) {
    ASSERT_FLOAT_EQ(2, myHisto.closestMean(x, false));
  }

  // looking for values in other bins also returns the same value
  for (Histogram::Real x = 100; x < 120; x += 0.1) {
    ASSERT_FLOAT_EQ(2, myHisto.closestMean(x, false));
  }
  for (Histogram::Real x = 130; x < 150; x += 0.1) {
    ASSERT_FLOAT_EQ(2, myHisto.closestMean(x, false));
  }

  // add values to the first bin (#0)
  myHisto(100, 4);

  // looking for values in the first bin returns the exact value
  ASSERT_FLOAT_EQ(4, myHisto.closestMean(105, false));

  // looking for values in the second bin returns an interpolation
  ASSERT_FLOAT_EQ(3, myHisto.closestMean(115, false));

  // looking for values in the fourth and fifth bin return the middle bin value
  for (Histogram::Real x = 130; x < 150; x += 0.1) {
    ASSERT_FLOAT_EQ(2, myHisto.closestMean(x, false));
  }
}

TEST_F(TestHistogram, test_closestmean_interp) {
  // create an empty histogram and put values in bin #0 and #4
  Histogram myHisto(100, 10, 5, Histogram::IGNORE);
  myHisto(105, 1);
  myHisto(145, 5);

  // exact values when available
  for (Histogram::Real x = 100; x < 110; x += 0.1) {
    EXPECT_FLOAT_EQ(1, myHisto.closestMean(x, false)) << x;
  }
  for (Histogram::Real x = 140; x < 150; x += 0.1) {
    EXPECT_FLOAT_EQ(5, myHisto.closestMean(x, false)) << x;
  }
  // linear interpolation otherwise
  for (Histogram::Real x = 110; x < 140; x += 0.1) {
    EXPECT_FLOAT_EQ(1 + (x - 105) / 10, myHisto.closestMean(x, false)) << x;
  }
}

} // namespace support
} // namespace uiiit
