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

  // call superfast
  for (auto i = 0; i < 100; i++) {
    const auto myCurLoad = myLoad();
    EXPECT_GE(myCurLoad.first, 0.0);
    EXPECT_LE(myCurLoad.first, 1.0);
    EXPECT_GE(myCurLoad.second, 0.0);
    EXPECT_LE(myCurLoad.second, 1.0);
  }
}

} // namespace support
} // namespace uiiit