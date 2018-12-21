/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2018 Claudio Cicconetti <https://about.me/ccicconetti>

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
