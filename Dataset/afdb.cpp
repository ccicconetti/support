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

//
// https://github.com/Azure/AzurePublicDataset/blob/master/AzureFunctionsBlobDataset2020.md
//

#include "Dataset/afdb-utils.h"
#include "Support/glograii.h"
#include "Support/split.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>

#include <glog/logging.h>

#include <cassert>
#include <deque>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace po = boost::program_options;
namespace us = uiiit::support;
namespace ud = uiiit::dataset;

struct Period {
  std::vector<double>      theReadDurations;
  std::vector<double>      theWriteDurations;
  std::vector<std::size_t> theReadEvents;
  std::vector<std::size_t> theWriteEvents;
};

std::size_t
readWritePeriods(const std::deque<ud::Row>&               aDataset,
                 std::unordered_map<std::string, Period>& aPeriods) {
  // largest vector
  std::size_t ret = 0;

  // key: user id + app id
  // value: 0: last timestamp
  //        1: last write flag
  //        2: consecutive events
  std::unordered_map<std::string, std::tuple<double, bool, std::size_t>> myLast;
  for (const auto& myRow : aDataset) {
    const auto myKey = myRow.key();
    auto       res   = myLast.emplace(
        myKey, std::make_tuple(myRow.theTimestamp, myRow.theWrite, 1));

    if (not res.second) {
      if (std::get<1>(res.first->second) == myRow.theWrite) {
        // we are in the middle of a period of consecutive read/write operations
        std::get<2>(res.first->second)++;

      } else {
        // the read/write period just ended
        auto& myPeriod = aPeriods[myKey];

        auto& myDurations = myRow.theWrite ? myPeriod.theReadDurations :
                                             myPeriod.theWriteDurations;
        myDurations.emplace_back(myRow.theTimestamp -
                                 std::get<0>(res.first->second));

        auto& myEvents =
            myRow.theWrite ? myPeriod.theReadEvents : myPeriod.theWriteEvents;
        myEvents.emplace_back(std::get<2>(res.first->second));

        ret = std::max(ret, myDurations.size());

        std::get<0>(res.first->second) = myRow.theTimestamp;
        std::get<1>(res.first->second) = myRow.theWrite;
        std::get<2>(res.first->second) = 1;
      }
    }
  }

  return ret;
}

void savePeriods(std::unordered_map<std::string, Period>& aPeriods,
                 const boost::filesystem::path&           aOutputPath) {
  std::size_t myCounter = 0;
  for (const auto& myPeriod : aPeriods) {
    myCounter++;
    // clang-format off
    std::ofstream myRDStream((aOutputPath / (std::string("read-durations-")+std::to_string(myCounter)+".dat")).string());
    std::ofstream myWDStream((aOutputPath / (std::string("write-durations-")+std::to_string(myCounter)+".dat")).string());
    std::ofstream myREStream((aOutputPath / (std::string("read-events-")+std::to_string(myCounter)+".dat")).string());
    std::ofstream myWEStream((aOutputPath / (std::string("write-events-")+std::to_string(myCounter)+".dat")).string());
    // clang-format on
    for (const auto& myValue : myPeriod.second.theReadDurations) {
      myRDStream << myValue << '\n';
    }
    for (const auto& myValue : myPeriod.second.theWriteDurations) {
      myWDStream << myValue << '\n';
    }
    for (const auto& myValue : myPeriod.second.theReadEvents) {
      myREStream << myValue << '\n';
    }
    for (const auto& myValue : myPeriod.second.theWriteEvents) {
      myWEStream << myValue << '\n';
    }
  }
}

void saveNumInvocations(const std::deque<ud::Row>&     aDataset,
                        const boost::filesystem::path& aOutputPath) {
  std::unordered_map<std::string, std::size_t> myNumInvocations;
  for (const auto& myRow : aDataset) {
    auto it = myNumInvocations.emplace(myRow.key(), 0);
    it.first->second++;
  }

  std::ofstream myStream(
      (aOutputPath / (std::string("num-invocations.dat"))).string());

  for (const auto& elem : myNumInvocations) {
    myStream << elem.first << ',' << elem.second << '\n';
  }
}

int main(int argc, char* argv[]) {
  us::GlogRaii myGlogRaii(argv[0]);

  std::string myDatasetFilename;
  std::string myOutputDir;
  std::string myAnalysis;

  po::options_description myDesc("Allowed options");
  // clang-format off
  myDesc.add_options()
    ("help,h", "produce help message")
    ("input-dataset",
     po::value<std::string>(&myDatasetFilename)->default_value(""),
     "Input dataset file name.")
    ("output-dir",
     po::value<std::string>(&myOutputDir)->default_value(""),
     "Output directory.")
    ("analysis",
     po::value<std::string>(&myAnalysis)->default_value("num-invocations"),
     "Type of analysis, one of: {read-write-periods, num-invocations}.")
    ;
  // clang-format on

  try {
    po::variables_map myVarMap;
    po::store(po::parse_command_line(argc, argv, myDesc), myVarMap);
    po::notify(myVarMap);

    if (myVarMap.count("help")) {
      std::cout << myDesc << std::endl;
      return EXIT_FAILURE;
    }

    VLOG(1) << "reading from: " << myDatasetFilename;
    std::ifstream myDatasetStream(myDatasetFilename);
    if (not myDatasetStream) {
      throw std::runtime_error("could not open dataset file for reading: " +
                               myDatasetFilename);
    }
    auto myDataset = ud::loadDataset(myDatasetStream, true);

    VLOG(1) << "analyzing dataset";
    if (myAnalysis == "read-write-periods") {
      std::unordered_map<std::string, Period> myPeriods;
      [[maybe_unused]] const auto             myMaxValues =
          readWritePeriods(myDataset, myPeriods);

      VLOG(1) << "writing output";
      savePeriods(myPeriods, myOutputDir);

    } else if (myAnalysis == "num-invocations") {
      saveNumInvocations(myDataset, myOutputDir);

    } else {
      throw std::runtime_error("Invalid type of analysis: " + myAnalysis);
    }

    VLOG(1) << "done";

    return EXIT_SUCCESS;
  } catch (const std::exception& aErr) {
    std::cerr << "Exception caught: " << aErr.what() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }

  return EXIT_FAILURE;
}
