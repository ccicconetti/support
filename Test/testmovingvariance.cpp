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
