#include "Support/random.h"

#include "gtest/gtest.h"

#include <glog/logging.h>

namespace uiiit {
namespace support {

struct TestRandom : public ::testing::Test {};

template <class RV>
void test_mean(RV& aRv, const double aExpectedMean) {
  double mySum = 0;
  for (auto i = 0; i < 10000; i++) {
    mySum += aRv();
  }
  const auto myActualMean = mySum / 10000;
  ASSERT_NEAR(aExpectedMean, myActualMean, 0.1);
}

template <class RV, class CONTAINER>
void fill(RV&& aRv, CONTAINER& aContainer) {
  for (auto& elem : aContainer) {
    elem = aRv();
  }
}

TEST_F(TestRandom, test_random) {
  auto a = random();
  auto b = random();
  ASSERT_NE(a, b);
  LOG(INFO) << "random value today is " << a;
}

TEST_F(TestRandom, test_uniform) {
  ASSERT_THROW(UniformRv(1, 0, 0, 0, 0), std::runtime_error);
  UniformRv myRv(-10, 10, 0, 0, 0);
  test_mean(myRv, 0);
}

TEST_F(TestRandom, test_repeatable) {
  std::vector<double> myVec0(10);
  std::vector<double> myVec1(10);
  std::vector<double> myVec2(10);
  std::vector<double> myVec3(10);
  std::vector<double> myVec4(10);

  fill(UniformRv(-10, 10, 1, 2, 3), myVec0);
  fill(UniformRv(-10, 10, 0, 2, 3), myVec1);
  fill(UniformRv(-10, 10, 1, 0, 3), myVec2);
  fill(UniformRv(-10, 10, 1, 2, 0), myVec3);
  fill(UniformRv(-10, 10, 1, 2, 3), myVec4);

  ASSERT_NE(myVec0, myVec1);
  ASSERT_NE(myVec0, myVec2);
  ASSERT_NE(myVec0, myVec3);
  ASSERT_EQ(myVec0, myVec4);
}

TEST_F(TestRandom, test_uniform_int) {
  ASSERT_THROW(UniformRv(1, 0, 0, 0, 0), std::runtime_error);
  {
    UniformIntRv<int> myRv(-10, 10, 0, 0, 0);
    test_mean(myRv, 0);
  }
  {
    UniformIntRv<unsigned int> myRv(0, 10, 0, 0, 0);
    test_mean(myRv, 5);
  }
}

TEST_F(TestRandom, test_exponential) {
  ExponentialRv myRv(0.1, 0, 0, 0);
  test_mean(myRv, 10);
}

TEST_F(TestRandom, test_poisson) {
  PoissonRv myRv(10, 0, 0, 0);
  test_mean(myRv, 10);
}

} // namespace support
} // namespace uiiit
