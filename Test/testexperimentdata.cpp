/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2022 Claudio Cicconetti https://ccicconetti.github.io/

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

#include "Support/experimentdata.h"
#include "Support/split.h"

#include "gtest/gtest.h"

#include <glog/logging.h>

#include <sstream>
#include <string>

namespace uiiit {
namespace support {

struct TestExperimentData : public ::testing::Test {
  struct IN {
    int         x;
    std::string toCsv() const {
      return std::to_string(x);
    }
    std::string toString() const {
      return "x = " + std::to_string(x);
    }
  };
  struct OUT {
    double      y;
    std::string z;
    std::string toCsv() const {
      return std::to_string(y) + "," + z;
    }
    std::string toString() const {
      return "y = " + std::to_string(y) + " " + z;
    }
  };
  using Raii = ExperimentData<IN, OUT>::Raii;
};

TEST_F(TestExperimentData, test_ctor) {
  struct EMPTY_IN {};
  struct EMPTY_OUT {};
  ExperimentData<EMPTY_IN, EMPTY_OUT> myExperimentData;
}

TEST_F(TestExperimentData, test_no_finish) {
  ExperimentData<IN, OUT> myExperimentData;
  {
    Raii myRaii(myExperimentData, {0});
    // no call to finish
  }
  ASSERT_EQ(1, myExperimentData.size());
  ASSERT_EQ("x = 0", myExperimentData.lastIn());
  ASSERT_EQ("y = 0.000000 ", myExperimentData.lastOut());
}

TEST_F(TestExperimentData, test_csv) {
  ExperimentData<IN, OUT> myExperimentData;

  {
    Raii myRaii(myExperimentData, {0});
    myRaii.finish({3.14, "out0"});
    ASSERT_EQ(0, myExperimentData.size());
  }
  ASSERT_EQ(1, myExperimentData.size());

  {
    IN   myIn{1};
    OUT  myOut{6.28, "out1"};
    Raii myRaii(myExperimentData, std::move(myIn));
    myRaii.finish(std::move(myOut));
  }

  ASSERT_FLOAT_EQ(6.28, myExperimentData.peek().y);
  ASSERT_EQ(2, myExperimentData.size());

  // save to CSV
  std::stringstream myStream;
  myExperimentData.toCsv(myStream);

  std::string myLine;
  std::size_t myCounter = 0;
  while (std::getline(myStream, myLine)) {
    VLOG(1) << myLine;
    const auto myTokens = split<std::vector<std::string>>(myLine, ",");
    ASSERT_EQ(4, myTokens.size());
    ASSERT_EQ(myCounter, std::stoi(myTokens[0]));
    ASSERT_FLOAT_EQ(3.14 * (1 + myCounter), std::stod(myTokens[1]));
    ASSERT_EQ("out" + std::to_string(myCounter), myTokens[2]);
    ASSERT_GT(std::stod(myTokens[3]), 0);
    myCounter++;
  }
}

} // namespace support
} // namespace uiiit
