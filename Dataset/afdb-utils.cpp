/*
              __ __ __
             |__|__|  | __
             |  |  |  ||__|
  ___ ___ __ |  |  |  |
 |   |   |  ||  |  |  |    Ubiquitous Internet @ IIT-CNR
 |   |   |  ||  |  |  |    C++ support library
 |_______|__||__|__|__|    https://github.com/ccicconetti/support

Licensed under the MIT License <http://opensource.org/licenses/MIT>
Copyright (c) 2022 C. Cicconetti <https://ccicconetti.github.io/>

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

#include "Dataset/afdb-utils.h"

#include <iterator>
#include <sstream>
#include <stdexcept>

namespace uiiit {
namespace dataset {

Row::Row(const std::string& aRow) {
  auto myTokens = support::split<std::vector<std::string>>(aRow, ",");
  if (myTokens.size() != 11) {
    throw std::runtime_error("Wrong number of elements in row: " + aRow);
  }
  theTimestamp = std::stod(myTokens[0]);
  theRegion.swap(myTokens[1]);
  theUser.swap(myTokens[2]);
  theApp.swap(myTokens[3]);
  theFunction.swap(myTokens[4]);
  theBlob.swap(myTokens[5]);
  theBlobType.swap(myTokens[6]);
  theBlobVersion.swap(myTokens[7]);
  theBlobSize = std::stoull(myTokens[8]);
  if (myTokens[9] == "True" and myTokens[10].size() >= 5 and
      myTokens[10].substr(0, 5) == "False") {
    theWrite = false;
  } else if (myTokens[9] == "False" and myTokens[10].size() >= 4 and
             myTokens[10].substr(0, 4) == "True") {
    theWrite = true;
  } else {
    throw std::runtime_error("Invalid read/write flags in row: " + aRow);
  }
}

std::deque<Row> loadDataset(std::istream& aStream, const bool aWithHeader) {
  std::size_t myRowId = 0;
  std::string myLine;
  if (aWithHeader) {
    std::getline(aStream, myLine);
    if (myLine.empty()) {
      throw std::runtime_error("Invalid empty header");
    }
  }
  std::deque<Row> ret;
  while (aStream) {
    ++myRowId;
    try {
      std::getline(aStream, myLine);
      if (myLine.empty()) {
        break;
      }
      ret.emplace_back(Row(myLine));
    } catch (const std::exception& aErr) {
      LOG(ERROR) << "error reading line " << myRowId << ": " << aErr.what();
    }
  }
  return ret;
}

std::string toString(const ExecMode aExecMode) {
  // clang-format off
  switch (aExecMode) {
    case ExecMode::AlwaysMu:     return "always-mu";
    case ExecMode::AlwaysLambda: return "always-lambda";
    case ExecMode::BestNext:     return "best-next";
    default: throw std::runtime_error("unknown");
  }
  // clang-format on
}

const std::list<ExecMode>& allExecModes() {
  static const std::list<ExecMode> myExecModes({
      ExecMode::AlwaysMu,
      ExecMode::AlwaysLambda,
      ExecMode::BestNext,
  });
  return myExecModes;
}

std::string CostModel::toString() const {
  std::stringstream ret;
  ret << theCostExecMu << ',' << theCostExecLambda << ',' << theCostWarmMu
      << ',' << theCostWarmLambda << ',' << theCostMigrateMu << ','
      << theCostMigrateLambda;

  return ret.str();
}

std::unordered_map<std::string, std::vector<double>>
cost(const std::deque<Row>& aDataset, const CostModel& aCostModel) {
  // group the rows by key and store only the timestamps
  std::unordered_map<std::string, std::deque<double>> myTimestamps;
  for (const auto& myRow : aDataset) {
    myTimestamps[myRow.key()].emplace_back(myRow.theTimestamp);
  }

  // compute the costs
  std::unordered_map<std::string, std::vector<double>> ret;
  for (const auto& elem : myTimestamps) {
    auto& myCost =
        ret.emplace(elem.first,
                    std::vector<double>(
                        static_cast<unsigned int>(ExecMode::Size), 0))
            .first->second;
    for (auto cur = elem.second.begin(); cur != elem.second.end(); ++cur) {
      auto next = std::next(cur);
      if (next == elem.second.end()) {
        break;
      }

      myCost[static_cast<unsigned int>(ExecMode::AlwaysMu)] +=
          aCostModel.theCostExecMu + aCostModel.theCostWarmMu * (*next - *cur);
    }
  }

  return ret;
}

} // namespace dataset
} // namespace uiiit