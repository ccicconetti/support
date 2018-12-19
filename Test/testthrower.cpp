#include "gtest/gtest.h"

#include "Support/thrower.h"

namespace uiiit {
namespace support {

struct TestThrower : public ::testing::Test {};

TEST_F(TestThrower, test_usage) {
  Thrower myThrower("hdr: ");
  ASSERT_NO_THROW(myThrower.check());

  myThower(true, "err1");
  ASSERT_NO_THROW(myThrower.check());

  myThower(false, "err2");
  myThower(true, "err3");
  ASSERT_THROW(myThrower.check(), std::runtime_error);

  try {
    myThrower.check();
  } catch (const std::exception& aErr) {
    ASSERT_EQ("hdr: err1, err3", aErr.what());
  }
}

} // namespace support
} // namespace uiiit
