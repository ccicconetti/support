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
