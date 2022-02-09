/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/support/
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

#include "Support/macros.h"
#include "Support/random.h"

#include "gtest/gtest.h"

#include <glog/logging.h>

#include <list>
#include <stdexcept>
#include <vector>

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

TEST_F(TestRandom, test_choice) {
  UniformRv myRv(0, 1, 0, 0, 0);

  {
    std::vector<int> myVector({0, 1, 2, 3});
    std::set<int>    myFound;
    for (size_t i = 0; i < 100; i++) {
      myFound.emplace(choice(myVector, myRv));
    }
    ASSERT_EQ(std::set<int>({0, 1, 2, 3}), myFound);
  }

  {
    std::list<std::string> myList({"Mickey", "Goofy", "Donald"});
    std::set<std::string>  myFound;
    for (size_t i = 0; i < 100; i++) {
      myFound.emplace(choice(myList, myRv));
    }
    ASSERT_EQ(std::set<std::string>({"Mickey", "Goofy", "Donald"}), myFound);
  }

  ASSERT_EQ(42, choice(std::vector<int>({42}), myRv));

  ASSERT_THROW(choice(std::vector<int>(), myRv), std::runtime_error);
}

TEST_F(TestRandom, test_sample) {
  UniformRv myRv(0, 1, 0, 0, 0);

  std::set<int> myVector({0, 1, 2, 3});
  ASSERT_EQ(myVector, sample(myVector, 4, myRv));
  ASSERT_EQ(myVector, sample(myVector, 99, myRv));

  std::set<int> myFound;
  for (size_t i = 0; i < 100; i++) {
    const auto mySample = sample(myVector, 2, myRv);
    ASSERT_EQ(2, mySample.size());
    for (const auto& elem : mySample) {
      myFound.emplace(elem);
    }
  }
  ASSERT_EQ(myVector, myFound);
}

} // namespace support
} // namespace uiiit
