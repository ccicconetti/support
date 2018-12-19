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
