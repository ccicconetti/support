#include "Support/fit.h"

#include "gtest/gtest.h"

#include <cmath>

namespace uiiit {
namespace support {

struct TestFit : public ::testing::Test {};

TEST_F(TestFit, test_special_cases) {
  using Vector = std::vector<std::pair<float, float>>;

  // no values
  ASSERT_EQ((std::pair<float, float>(0, 0)), fit(Vector({})));

  // single value
  ASSERT_EQ((std::pair<float, float>(0, 0)), fit(Vector({{0, 0}})));
  ASSERT_EQ((std::pair<float, float>(0, 0)), fit(Vector({{1, 0}})));
  ASSERT_EQ((std::pair<float, float>(1, 0)), fit(Vector({{0, 1}})));
  ASSERT_EQ((std::pair<float, float>(1, 0)), fit(Vector({{1, 1}})));

  // multiple values, same abscissa
  Vector myVector;
  for (size_t i = 0; i < 100; i++) {
    myVector.emplace_back(std::make_pair(-42.0f, static_cast<float>(i)));
  }
  ASSERT_EQ((std::pair<float, float>(49.5, 0)), fit(myVector));

  // add one single different value
  myVector.emplace_back(std::make_pair(-41.0f, 100.0f));
  const auto ret = fit(myVector);
  EXPECT_FLOAT_EQ(2170.8962, ret.first);
  EXPECT_FLOAT_EQ(50.509438, ret.second);
}

TEST_F(TestFit, test_valid_float) {
  using Vector = std::vector<std::pair<float, float>>;

  {
    const auto ret = fit(Vector({{2, 0}, {3, 1}}));
    EXPECT_FLOAT_EQ(-2, ret.first);
    EXPECT_FLOAT_EQ(1, ret.second);
  }

  {
    Vector     myVector({
        {0.28533, 285.322},
        {2.28057, 280.516},
        {4.29102, 290.224},
        {6.29523, 294.152},
        {8.29327, 291.654},
        {10.3628, 356.357},
        {12.3393, 331.829},
        {14.3074, 298.476},
        {16.396, 385.218},
        {18.444, 429.69},
    });
    const auto ret = fit(myVector);
    EXPECT_FLOAT_EQ(260.95938, ret.first);
    EXPECT_FLOAT_EQ(6.7939849, ret.second);
  }
}

TEST_F(TestFit, test_twopoints_invalid) {
  using Type = float;

  const auto ret = fit<Type>(42, 1, 42, -9);
  EXPECT_EQ(0, ret.first);
  EXPECT_EQ(0, ret.second);
}

TEST_F(TestFit, test_twopoints_valid) {
  using Type = double;

  const Type a{-3.14};
  const Type b{42};

  for (Type x = -100; x < 100; x++) {
    const auto x1  = x;
    const auto x2  = x + 1;
    const auto y1  = a + b * x1;
    const auto y2  = a + b * x2;
    const auto ret = fit<Type>(x1, y1, x2, y2);
    EXPECT_NEAR(a, ret.first, 0.01);
    EXPECT_NEAR(b, ret.second, 0.01);
  }
}

TEST_F(TestFit, test_extrapolate) {
  using Type = double;

  const Type a{-3.14};
  const Type b{42};

  for (Type x = -100; x < 100; x++) {
    const auto x1 = x;
    const auto x2 = x + 1;
    const auto X  = x + 2;
    const auto y1 = a + b * x1;
    const auto y2 = a + b * x2;
    const auto Y  = a + b * X;
    EXPECT_NEAR(Y, extrapolate<Type>(x1, y1, x2, y2, X), 0.01);
  }
}

} // namespace support
} // namespace uiiit
