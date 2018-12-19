#include "Support/system.h"

#include <glog/logging.h>

#include "gtest/gtest.h"

namespace uiiit {
namespace support {

struct TestSystem : public ::testing::Test {};

TEST_F(TestSystem, test_hostname_nothrow) {
  std::string myHostName;
  ASSERT_NO_THROW(myHostName = System::instance().hostName());
  ASSERT_TRUE(not myHostName.empty());
  LOG(INFO) << "host name: " << myHostName;
}

TEST_F(TestSystem, test_cpuname_nothrow) {
  std::string myCpuName;
  ASSERT_NO_THROW(myCpuName = System::instance().cpuName());
  ASSERT_TRUE(not myCpuName.empty());
  LOG(INFO) << "CPU model name: " << myCpuName;
}

} // namespace support
} // namespace uiiit
