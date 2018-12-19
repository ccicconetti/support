#include "gtest/gtest.h"

#include "Support/chrono.h"
#include "Support/wait.h"
#include "Test/macros.h"

namespace uiiit {
namespace support {

struct TestWait : public ::testing::Test {};

TEST_F(TestWait, test_waitfor) {
  ASSERT_TRUE(support::waitFor<int>([]() { return 7; }, 7, 0.1));

  Chrono myChrono(true);
  ASSERT_FALSE(support::waitFor<int>([]() { return 8; }, 7, 0.1));
  const auto myElapsed = myChrono.stop();
  ASSERT_GT(myElapsed, 0.1);

  int myCounter = 0;
  ASSERT_TRUE((
      support::waitFor<int>([&myCounter]() { return myCounter++; }, 100, 1.0)));

  WAIT_FOR([]() { return true; }, 0.1) << "should not happen";
}

} // namespace support
} // namespace uiiit
