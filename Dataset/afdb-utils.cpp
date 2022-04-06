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
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>

namespace uiiit {
namespace dataset {

const std::size_t theTimestampDatasetVersion = 1;

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
  ret << theCostExecMu << ',' << theCostExecLambda << ',' << theCostReadLambda
      << ',' << theCostWriteLambda << ',' << theCostWarmMu << ','
      << theCostWarmLambda << ',' << theCostMigrateMu << ','
      << theCostMigrateLambda;

  return ret.str();
}

const std::vector<std::string>& CostModel::explain() {
  static const std::vector<std::string> myExplain({
      "cost-exec-mu",
      "cost-exec-lambda",
      "cost-read-lambda",
      "cost-write-lambda",
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

TimestampDataset toTimestampDataset(const std::deque<Row>& aDataset) {
  TimestampDataset ret;
  for (const auto& myRow : aDataset) {
    ret[myRow.key()].emplace_back(myRow.theTimestamp, myRow.theWrite);
  }
  return ret;
}

struct EndOfStream final : public std::runtime_error {
  explicit EndOfStream()
      : std::runtime_error("EOF") {
    // noop
  }
};

template <typename T>
void readFromFile(std::istream& aStream, T& aValue) {
  aStream.read(reinterpret_cast<char*>(&aValue), sizeof(aValue));
  if (not aStream) {
    throw EndOfStream();
  }
}

TimestampDataset loadTimestampDataset(const std::string& aFilename) {
  std::ifstream myInfile(aFilename);
  if (not myInfile) {
    throw std::runtime_error("Could not open file for reading: " + aFilename);
  }
  TimestampDataset ret;

  // load version number
  std::size_t myVersion;
  readFromFile(myInfile, myVersion);
  if (myVersion != theTimestampDatasetVersion) {
    throw std::runtime_error("Invalid file version: expecting " +
                             std::to_string(theTimestampDatasetVersion) +
                             ", found " + std::to_string(myVersion));
  }

  try {
    std::size_t myLength;
    double      myTimestamp;
    bool        myWriteFlag;
    while (true) {
      // read length, then the key
      readFromFile(myInfile, myLength);
      std::string myKey(myLength, 0);
      myInfile.read(myKey.data(), myLength);

      // read the number of elements, then all the elements
      readFromFile(myInfile, myLength);
      auto myNewElem =
          ret.emplace(myKey, std::deque<std::tuple<double, bool>>());
      assert(myNewElem.second == true);
      for (std::size_t i = 0; i < myLength; i++) {
        readFromFile(myInfile, myTimestamp);
        readFromFile(myInfile, myWriteFlag);
        myNewElem.first->second.emplace_back(myTimestamp, myWriteFlag);
      }
    }
  } catch (const EndOfStream&) {
    // ignore
  }

  return ret;
}

void saveTimestampDataset(const TimestampDataset& aDataset,
                          const std::string&      aFilename) {
  std::ofstream myOutfile(aFilename);
  if (not myOutfile) {
    throw std::runtime_error("Could not open file for writing: " + aFilename);
  }

  // write version number
  myOutfile.write(reinterpret_cast<const char*>(&theTimestampDatasetVersion),
                  sizeof(std::size_t));

  std::size_t myLength;
  for (const auto& myApp : aDataset) {
    // write size of the key, then the key
    myLength = myApp.first.size();
    myOutfile.write(reinterpret_cast<const char*>(&myLength), sizeof(myLength));
    myOutfile.write(myApp.first.data(), myLength);

    // write then number of elements, then all the elements
    myLength = myApp.second.size();
    myOutfile.write(reinterpret_cast<const char*>(&myLength), sizeof(myLength));
    for (const auto& elem : myApp.second) {
      myOutfile.write(reinterpret_cast<const char*>(&std::get<0>(elem)),
                      sizeof(std::get<0>(elem)));
      myOutfile.write(reinterpret_cast<const char*>(&std::get<1>(elem)),
                      sizeof(std::get<1>(elem)));
    }
  }
}

std::unordered_map<std::string, CostOutput>
cost(const TimestampDataset& aDataset,
     const CostModel&        aCostModel,
     const bool              aSaveBnPeriods) {
  const std::size_t myBestNextLookAhead = 500;

  struct PeriodSaver {
    PeriodSaver(CostOutput& aCost, const bool aSave)
        : theCost(aCost)
        , theSave(aSave)
        , theAcc(0) {
      // noop
    }
    void remain(const double aPeriod) {
      theAcc += aPeriod;
    }
    void migrate(const double aPeriod) {
      if (theSave) {
        theCost.theBestNextPeriods.emplace_back(theAcc);
      }
      theAcc = aPeriod;
    }

    CostOutput& theCost;
    const bool  theSave;
    double      theAcc;
  };

  // compute the costs
  std::unordered_map<std::string, CostOutput> ret;
  for (const auto& elem : aDataset) {
    auto&       myOut = ret.emplace(elem.first, CostOutput()).first->second;
    PeriodSaver myPeriodSaver(myOut, aSaveBnPeriods);
    for (auto cur = elem.second.begin(); cur != elem.second.end(); ++cur) {
      auto next = std::next(cur);
      if (next == elem.second.end()) {
        break;
      }

      const auto myTimeToNext = std::get<0>(*next) - std::get<0>(*cur);
      myOut.theDuration += myTimeToNext;
      myOut.theNumInvocations++;

      // AlwaysMu
      myOut.theCosts[static_cast<unsigned int>(ExecMode::AlwaysMu)] +=
          aCostModel.theCostExecMu + aCostModel.theCostWarmMu * myTimeToNext;

      // AlwaysLambda
      myOut.theCosts[static_cast<unsigned int>(ExecMode::AlwaysLambda)] +=
          aCostModel.theCostExecLambda +
          aCostModel.theCostWarmLambda * myTimeToNext +
          (std::get<1>(*cur) ? aCostModel.theCostWriteLambda :
                               aCostModel.theCostReadLambda);

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
          myPeriodSaver.migrate(myTimeToNext);
          myOut.theBestNextLastType = CostOutput::Type::Stateless;
          myOut.theBestNextDurLambda += myTimeToNext;
          myBnCost += aCostModel.theCostMigrateLambda +
                      aCostModel.theCostWarmLambda * myTimeToNext;

        } else {
          myPeriodSaver.remain(myTimeToNext);
          myOut.theBestNextDurMu += myTimeToNext;
          myBnCost += aCostModel.theCostWarmMu * myTimeToNext;
        }

      } else {
        assert(myOut.theBestNextLastType == CostOutput::Type::Stateless);

        auto myCostMu = aCostModel.theCostMigrateMu + aCostModel.theCostExecMu;
        auto myCostLambda = aCostModel.theCostExecLambda +
                            (std::get<1>(*cur) ? aCostModel.theCostWriteLambda :
                                                 aCostModel.theCostReadLambda);
        auto        myLast = std::get<0>(*cur);
        std::size_t i      = 0;
        for (auto it = next;
             it != elem.second.end() and i < myBestNextLookAhead;
             ++it, ++i) {
          const auto myDuration = std::get<0>(*it) - myLast;
          myCostMu +=
              aCostModel.theCostExecMu + aCostModel.theCostWarmMu * myDuration;
          myCostLambda += aCostModel.theCostExecLambda +
                          (std::get<1>(*it) ? aCostModel.theCostWriteLambda :
                                              aCostModel.theCostReadLambda) +
                          aCostModel.theCostWarmLambda * myDuration;
          myLast = std::get<0>(*it);

          if (myCostMu < myCostLambda) {
            break;
          }
        }

        if (myCostMu < myCostLambda) {
          // migrate from stateless to microservice
          myPeriodSaver.migrate(myTimeToNext);
          myOut.theBestNextLastType = CostOutput::Type::Microservice;
          myOut.theBestNextNumMu++;
          myOut.theBestNextDurMu += myTimeToNext;
          myBnCost += aCostModel.theCostMigrateMu + aCostModel.theCostExecMu +
                      aCostModel.theCostWarmMu * myTimeToNext;

        } else {
          myPeriodSaver.remain(myTimeToNext);
          myOut.theBestNextNumLambda++;
          myOut.theBestNextDurLambda += myTimeToNext;
          myBnCost += aCostModel.theCostExecLambda +
                      (std::get<1>(*cur) ? aCostModel.theCostWriteLambda :
                                           aCostModel.theCostReadLambda) +
                      aCostModel.theCostWarmLambda * myTimeToNext;
        }
      }
    }
  }

  return ret;
}

} // namespace dataset
} // namespace uiiit
