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

#include <cassert>
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

const std::array<ExecMode, static_cast<unsigned int>(ExecMode::Size)>&
allExecModes() {
  static const std::array<ExecMode, static_cast<unsigned int>(ExecMode::Size)>
      myExecModes({
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

const std::vector<std::string>& CostModel::explain() {
  static const std::vector<std::string> myExplain({
      "cost-exec-mu",
      "cost-exec-lambda",
      "cost-warm-mu",
      "cost-warm-lambda",
      "cost-migrate-mu",
      "cost-migrate-lambda",
  });
  return myExplain;
}

std::string CostOutput::toString() const {
  std::stringstream ret;
  ret << theDuration << ',' << theNumInvocations;
  for (const auto& myCost : theCosts) {
    ret << ',' << myCost;
  }
  ret << ',' << theBestNextNumMu << ',' << theBestNextNumLambda << ','
      << theBestNextDurMu << ',' << theBestNextDurLambda;
  return ret.str();
}

const std::vector<std::string>& CostOutput::explain() {
  static std::vector<std::string> myExplain({
      "duration",
      "num-invocations",
  });
  if (myExplain.size() == 2) {
    for (const auto& myExecMode : allExecModes()) {
      myExplain.emplace_back(uiiit::dataset::toString(myExecMode));
    }
    myExplain.emplace_back("best-next-num-mu");
    myExplain.emplace_back("best-next-num-lambda");
    myExplain.emplace_back("best-next-dur-mu");
    myExplain.emplace_back("best-next-dur-lambda");
  }

  return myExplain;
}

std::unordered_map<std::string, CostOutput>
cost(const std::deque<Row>& aDataset, const CostModel& aCostModel) {
  const std::size_t myBestNextLookAhead = 100;

  // group the rows by key and store only the timestamps
  std::unordered_map<std::string, std::deque<double>> myTimestamps;
  for (const auto& myRow : aDataset) {
    myTimestamps[myRow.key()].emplace_back(myRow.theTimestamp);
  }

  // compute the costs
  std::unordered_map<std::string, CostOutput> ret;
  for (const auto& elem : myTimestamps) {
    auto& myOut = ret.emplace(elem.first, CostOutput()).first->second;
    for (auto cur = elem.second.begin(); cur != elem.second.end(); ++cur) {
      auto next = std::next(cur);
      if (next == elem.second.end()) {
        break;
      }

      const auto myTimeToNext = *next - *cur;
      myOut.theDuration += myTimeToNext;
      myOut.theNumInvocations++;

      // AlwaysMu
      myOut.theCosts[static_cast<unsigned int>(ExecMode::AlwaysMu)] +=
          aCostModel.theCostExecMu + aCostModel.theCostWarmMu * myTimeToNext;

      // AlwaysLambda
      myOut.theCosts[static_cast<unsigned int>(ExecMode::AlwaysLambda)] +=
          aCostModel.theCostExecLambda +
          aCostModel.theCostWarmLambda * myTimeToNext;

      // BestNext
      auto& myBnCost =
          myOut.theCosts[static_cast<unsigned int>(ExecMode::BestNext)];
      if (myOut.theBestNextLastType == CostOutput::Type::Microservice) {
        myBnCost += aCostModel.theCostExecMu;
        myOut.theBestNextNumMu++;

        if ((aCostModel.theCostWarmMu * myTimeToNext) >
            (aCostModel.theCostMigrateLambda +
             aCostModel.theCostWarmLambda * myTimeToNext)) {
          // migrate from microservice to stateless
          myOut.theBestNextLastType = CostOutput::Type::Stateless;
          myOut.theBestNextDurLambda += myTimeToNext;
          myBnCost += aCostModel.theCostMigrateLambda +
                      aCostModel.theCostWarmLambda * myTimeToNext;

        } else {
          myOut.theBestNextDurMu += myTimeToNext;
          myBnCost += aCostModel.theCostWarmMu * myTimeToNext;
        }

      } else {
        assert(myOut.theBestNextLastType == CostOutput::Type::Stateless);

        auto myCostMu = aCostModel.theCostMigrateMu + aCostModel.theCostExecMu;
        auto myCostLambda = aCostModel.theCostExecLambda;
        auto myLast       = *cur;
        std::size_t i     = 0;
        for (auto it = next;
             it != elem.second.end() and i < myBestNextLookAhead;
             ++it, ++i) {
          const auto myDuration = *it - myLast;
          myCostMu +=
              aCostModel.theCostExecMu + aCostModel.theCostWarmMu * myDuration;
          myCostLambda += aCostModel.theCostExecLambda +
                          aCostModel.theCostWarmLambda * myDuration;
          myLast = *it;

          if (myCostMu < myCostLambda) {
            break;
          }
        }

        if (myCostMu < myCostLambda) {
          // migrate from stateless to microservice
          myOut.theBestNextLastType = CostOutput::Type::Microservice;
          myOut.theBestNextNumMu++;
          myOut.theBestNextDurMu += myTimeToNext;
          myBnCost += aCostModel.theCostMigrateMu + aCostModel.theCostExecMu +
                      aCostModel.theCostWarmMu * myTimeToNext;

        } else {
          myOut.theBestNextNumLambda++;
          myOut.theBestNextDurLambda += myTimeToNext;
          myBnCost += aCostModel.theCostExecLambda +
                      aCostModel.theCostWarmLambda * myTimeToNext;
        }
      }
    }
  }

  return ret;
}

} // namespace dataset
} // namespace uiiit