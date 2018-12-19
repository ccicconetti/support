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
