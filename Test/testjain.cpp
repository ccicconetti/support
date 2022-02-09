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

#include "Support/jain.h"

#include "gtest/gtest.h"

#include <cmath>
#include <iterator>
#include <limits>
#include <stdexcept>

namespace uiiit {
namespace support {

struct TestJain : public ::testing::Test {

  double jain(const std::vector<double>& aValues) {
    assert(not aValues.empty());
    double myNum = 0;
    double myDem = 0;
    for (const auto& myValue : aValues) {
      myNum += myValue;
      myDem += myValue * myValue;
    }
    myNum *= myNum;
    myDem *= aValues.size();
    return myNum / myDem;
  }
};

TEST_F(TestJain, test_vector) {
  std::vector<double> myPopulation({1, 0.9, 0.8, 1, 1});
  ASSERT_FLOAT_EQ(jain(myPopulation), jainFairnessIndex(myPopulation));

  ASSERT_FLOAT_EQ(1, jainFairnessIndex(std::vector<double>({0.77})));

  ASSERT_THROW(jainFairnessIndex(std::vector<double>()), std::runtime_error);
}

TEST_F(TestJain, test_map) {
  std::map<std::string, int> myPopulation({
      {"1", 1},
      {"0.2", 0},
      {"0.9", 1},
      {"0.3", 0},
  });
  ASSERT_FLOAT_EQ(jain({1, 0.2, 0.9, 0.3}),
                  jainFairnessIndex(myPopulation, [](const auto& aPair) {
                    return std::stod(aPair.first);
                  }));
  ASSERT_FLOAT_EQ(0.5, jainFairnessIndex(myPopulation, [](const auto& aPair) {
                    return aPair.second;
                  }));
}

} // namespace support
} // namespace uiiit
