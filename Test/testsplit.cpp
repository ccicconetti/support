/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2019 Claudio Cicconetti https://ccicconetti.github.io/

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

#include "Support/split.h"

#include "gtest/gtest.h"

#include <set>
#include <vector>

namespace uiiit {
namespace support {

struct TestSplit : public ::testing::Test {};

TEST_F(TestSplit, test_empty) {
  using Container = std::vector<std::string>;
  ASSERT_TRUE(split<Container>("", ",").empty());
  ASSERT_TRUE(split<Container>(",", ",").empty());
  ASSERT_TRUE(split<Container>(",,", ",").empty());
}

TEST_F(TestSplit, test_strings) {
  using Container = std::vector<std::string>;
  ASSERT_EQ(Container({"a", "b", "pluto"}), split<Container>("a,b,pluto", ","));
  ASSERT_EQ(Container({"a", "b", "pluto"}),
            split<Container>(",a,b,,pluto,,,", ","));
  ASSERT_EQ(Container({"a", "b", "pluto"}),
            split<Container>("a:b;;pluto", ":;"));
  ASSERT_EQ(Container({"a", "b", "pluto"}), split<Container>("a b pluto", " "));
  ASSERT_EQ(Container({"a", "b", "pluto"}),
            split<Container>("a\tb\npluto", "\t\n"));
}

TEST_F(TestSplit, test_vector_floats) {
  using Container = std::vector<float>;

  const auto myResult = split<Container>("3.14,1e-3,.42e2", ",");
  ASSERT_EQ(3u, myResult.size());
  ASSERT_FLOAT_EQ(3.14f, myResult[0]);
  ASSERT_FLOAT_EQ(0.001, myResult[1]);
  ASSERT_FLOAT_EQ(42.0f, myResult[2]);
}

TEST_F(TestSplit, test_set_ints) {
  using Container = std::set<int>;

  ASSERT_EQ(Container({-1, 1, 42}),
            split<Container>("-1,-1,1,42,1,42,42", ","));
}

} // namespace support
} // namespace uiiit
