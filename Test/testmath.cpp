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
