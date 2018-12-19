#include "gtest/gtest.h"

#include "Support/uuid.h"

namespace uiiit {
namespace support {

struct TestUuid : ::testing::Test {};

TEST_F(TestUuid, test_usage) {
  Uuid myUuid1;
  Uuid myUuid2;

  ASSERT_NE(myUuid1, myUuid2);

  myUuid2 = myUuid1;
  ASSERT_EQ(myUuid1, myUuid2);
  ASSERT_EQ(myUuid1.toString(), myUuid2.toString());

  const auto myUuid3(myUuid1.toString());
  ASSERT_EQ(myUuid1.toString(), myUuid3);
}

} // namespace support
} // namespace uiiit
