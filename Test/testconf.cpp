#include "Support/conf.h"

#include "gtest/gtest.h"

namespace uiiit {
namespace support {

struct TestConf : public ::testing::Test {};

TEST_F(TestConf, test_string) {
  Conf myConf;

  myConf["key"] = "value";
  ASSERT_EQ("value", myConf("key"));

  ASSERT_TRUE(myConf("non-key").empty());

  myConf["non-key"] = "";
  ASSERT_TRUE(myConf("non-key").empty());

  myConf["non-key"] = "yes-value";
  ASSERT_EQ("yes-value", myConf("non-key"));

  myConf["key"] = "new-value";
  ASSERT_EQ("new-value", myConf("key"));
}

TEST_F(TestConf, test_double) {
  Conf myConf;

  ASSERT_THROW(myConf.getDouble("key"), NoValue);

  myConf["key"] = "3.14";
  ASSERT_FLOAT_EQ(3.14, myConf.getDouble("key"));

  myConf["key"] = "1";
  ASSERT_FLOAT_EQ(1, myConf.getDouble("key"));

  myConf["key"] = "aaa";
  ASSERT_THROW(myConf.getDouble("key"), InvalidDouble);
}

TEST_F(TestConf, test_int) {
  Conf myConf;

  ASSERT_THROW(myConf.getInt("key"), NoValue);

  myConf["key"] = "-1";
  ASSERT_EQ(-1, myConf.getInt("key"));

  myConf["key"] = "1";
  ASSERT_EQ(1, myConf.getInt("key"));

  myConf["key"] = "3.14";
  ASSERT_EQ(3, myConf.getInt("key"));

  myConf["key"] = "0.0";
  ASSERT_EQ(0, myConf.getInt("key"));

  myConf["key"] = "aaa";
  ASSERT_THROW(myConf.getInt("key"), InvalidInt);
}

TEST_F(TestConf, test_uint) {
  Conf myConf;

  ASSERT_THROW(myConf.getUint("key"), NoValue);

  myConf["key"] = "1";
  ASSERT_EQ(1, myConf.getUint("key"));

  myConf["key"] = "3.14";
  ASSERT_EQ(3, myConf.getUint("key"));

  myConf["key"] = "0.0";
  ASSERT_EQ(0, myConf.getUint("key"));

  myConf["key"] = "-1";
  ASSERT_THROW(myConf.getUint("key"), InvalidLogic);

  myConf["key"] = "-0.1";
  ASSERT_EQ(0, myConf.getUint("key"));

  myConf["key"] = "-0.99999";
  ASSERT_EQ(0, myConf.getUint("key"));

  myConf["key"] = "aaa";
  ASSERT_THROW(myConf.getUint("key"), InvalidInt);
}

TEST_F(TestConf, test_bool) {
  Conf myConf;

  ASSERT_THROW(myConf.getUint("key"), NoValue);

  myConf["key"] = "1";
  ASSERT_TRUE(myConf.getBool("key"));
  myConf["key"] = "true";
  ASSERT_TRUE(myConf.getBool("key"));
  myConf["key"] = "True";
  ASSERT_TRUE(myConf.getBool("key"));
  myConf["key"] = "TRUE";
  ASSERT_TRUE(myConf.getBool("key"));
  myConf["key"] = "yes";
  ASSERT_TRUE(myConf.getBool("key"));
  myConf["key"] = "Yes";
  ASSERT_TRUE(myConf.getBool("key"));
  myConf["key"] = "YES";
  ASSERT_TRUE(myConf.getBool("key"));

  myConf["key"] = "0";
  ASSERT_FALSE(myConf.getBool("key"));
  myConf["key"] = "false";
  ASSERT_FALSE(myConf.getBool("key"));
  myConf["key"] = "False";
  ASSERT_FALSE(myConf.getBool("key"));
  myConf["key"] = "FALSE";
  ASSERT_FALSE(myConf.getBool("key"));
  myConf["key"] = "no";
  ASSERT_FALSE(myConf.getBool("key"));
  myConf["key"] = "No";
  ASSERT_FALSE(myConf.getBool("key"));
  myConf["key"] = "NO";
  ASSERT_FALSE(myConf.getBool("key"));

  myConf["key"] = "x";
  ASSERT_THROW(myConf.getBool("key"), InvalidBool);
  myConf["key"] = "yessir";
  ASSERT_THROW(myConf.getBool("key"), InvalidBool);
  myConf["key"] = "nossir";
  ASSERT_THROW(myConf.getBool("key"), InvalidBool);
  myConf["key"] = "2";
  ASSERT_THROW(myConf.getBool("key"), InvalidBool);
  myConf["key"] = "-1";
  ASSERT_THROW(myConf.getBool("key"), InvalidBool);
  myConf["key"] = "0.0";
  ASSERT_THROW(myConf.getBool("key"), InvalidBool);
  myConf["key"] = "1.0";
  ASSERT_THROW(myConf.getBool("key"), InvalidBool);
  myConf["key"] = "3.14e0";
  ASSERT_THROW(myConf.getBool("key"), InvalidBool);
}

TEST_F(TestConf, test_from_list) {
  ASSERT_NO_THROW(Conf(""));

  ASSERT_NO_THROW(Conf("k=1,j=2,mickey=pluto,k e y=v a l u e"));

  ASSERT_NO_THROW(Conf("k=1,j=2,mickey=pluto,k e y=v a l u e,"));

  ASSERT_THROW(Conf("key="), InvalidConf);
  ASSERT_THROW(Conf("=value"), InvalidConf);
  ASSERT_THROW(Conf("="), InvalidConf);
  ASSERT_THROW(Conf("key=value,,"), InvalidConf);
  ASSERT_THROW(Conf(",key=value"), InvalidConf);
  ASSERT_THROW(Conf("key==value"), InvalidConf);
}

} // namespace support
} // namespace uiiit
