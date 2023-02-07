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
#include "Support/versionutils.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>

#include <glog/logging.h>

#include <cassert>
#include <deque>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace po = boost::program_options;
namespace us = uiiit::support;
namespace ud = uiiit::dataset;

std::unique_ptr<std::ofstream>
openFile(const std::string&             aFilename,
         const boost::filesystem::path& aOutputPath) {
  const auto myFilename = (aOutputPath / aFilename).string();
  auto       ret        = std::make_unique<std::ofstream>(myFilename);
  if (not *ret) {
    throw std::runtime_error("error when opening file for writing: " +
                             myFilename);
  }
  return ret;
}

//! Collects lifetime information of an application.
struct Lifecycle {
  //! The timestamp of the first event of the application.
  uint64_t theBegin = 0;
  //! The timestamp of the last event of the application.
  uint64_t theEnd = 0;
  //! The cumulative size of the write events.
  std::size_t theWrite = 0;
  //! The cumulative size of the read events.
  std::size_t theRead = 0;
  //! The total number of calls.
  std::size_t theCalls = 0;
  //! The total session time, in ms.
  uint64_t theSession = 0;

  //! \return true if this is the first time the Lifecyle was accessed.
  bool first() const noexcept {
    return theCalls == 0;
  }

  //! \return true if this application has been called only once.
  bool singleton() const noexcept {
    return theCalls == 1 or (theBegin == theEnd);
  }

  //! \return a string with values separated by commas.
  std::string toString() const {
    std::stringstream ret;
    ret << theBegin << ',' << theEnd << ',' << theWrite << ',' << theRead << ','
        << theCalls << ',' << theSession;
    return ret.str();
  }
};

using Lifecycles =
    std::unordered_map<std::string, std::unordered_map<std::string, Lifecycle>>;

/**
 * @brief Extract from the dataset lifecycle information about the apps.
 *
 * @param aDataset The input dataset.
 * @param aLifecycles The output data.
 * @param aSessionduration The session duration after the last call, in minutes.
 */
void lifecycles(const std::deque<ud::Row>& aDataset,
                Lifecycles&                aLifecycles,
                const double               aSessionduration) {
  assert(aLifecycles.empty());
  if (aDataset.empty()) {
    return;
  }
  const auto mySessionDuration =
      static_cast<uint64_t>(aSessionduration * 60 * 1000); // minutes -> ms
  const auto myTimeRef = static_cast<uint64_t>(aDataset.front().theTimestamp);
  for (const auto& myRow : aDataset) {
    auto& myPerRegion =
        aLifecycles.emplace(myRow.theRegion, Lifecycles::mapped_type())
            .first->second;
    auto& myLifecycle =
        myPerRegion.emplace(myRow.key(), Lifecycle()).first->second;
    const auto myTimestamp =
        static_cast<uint64_t>(myRow.theTimestamp) - myTimeRef;
    const auto myFirst = myLifecycle.first();
    if (myFirst) {
      myLifecycle.theBegin = myTimestamp;
    }
    const auto mySinceLast = myTimestamp - myLifecycle.theEnd;
    myLifecycle.theEnd     = myTimestamp;
    if (myRow.theWrite) {
      myLifecycle.theWrite += myRow.theBlobSize;
    } else {
      myLifecycle.theRead += myRow.theBlobSize;
    }
    myLifecycle.theCalls++;
    if (not myFirst) {
      myLifecycle.theSession += std::min(mySinceLast, mySessionDuration);
    }
  }

#ifndef NDEBUG
  // consistency checks
  for (const auto& myPerRegion : aLifecycles) {
    for (const auto& myPerApp : myPerRegion.second) {
      const auto& myLifecycle = myPerApp.second;
      assert(myLifecycle.theEnd >= myLifecycle.theBegin);
      assert(myLifecycle.singleton() or
             myLifecycle.theEnd > myLifecycle.theBegin);
      assert(not myLifecycle.first() or myLifecycle.theSession == 0);
    }
  }
#endif
}

/**
 * @brief Save to files info about the lifecycles of apps.
 *
 * @param aLifecycles The info data to be saved.
 * @param aOutputPath The directory where the data will be saved.
 * @param aSingletons If true then also save apps with a single function call.
 *
 * \pre aOutputPath exists and it is a directory
 */
void saveLifecycles(const Lifecycles&              aLifecycles,
                    const boost::filesystem::path& aOutputPath,
                    const bool                     aSingletons) {

  // save lifecycles, one per region
  for (const auto& myPerRegion : aLifecycles) {
    auto myStream = openFile(myPerRegion.first + ".dat", aOutputPath);
    for (const auto& myPerApp : myPerRegion.second) {
      if (aSingletons or not myPerApp.second.singleton()) {
        *myStream << myPerApp.first << ',' << myPerApp.second.toString()
                  << '\n';
      }
    }
  }

  // save number of concurrent apps, in total
  struct Event {
    enum Type : uint8_t {
      Begin = 0,
      End   = 1,
    };
    uint64_t theTimestamp = 0;
    Type     theType      = Type::Begin;
    bool     operator<(const Event& aOther) const noexcept {
      return theTimestamp < aOther.theTimestamp;
    }
  };

  std::multiset<Event> myEvents;
  for (const auto& myPerRegion : aLifecycles) {
    for (const auto& myPerApp : myPerRegion.second) {
      myEvents.emplace(Event{myPerApp.second.theBegin, Event::Begin});
      myEvents.emplace(Event{myPerApp.second.theEnd, Event::End});
    }
  }

  auto        myStream     = openFile("concurrent.dat", aOutputPath);
  std::size_t myConcurrent = 0;
  for (const auto& myEvent : myEvents) {
    if (myEvent.theType == Event::Begin) {
      myConcurrent++;
    } else {
      myConcurrent--;
    }
    *myStream << myEvent.theTimestamp << ' ' << myConcurrent << '\n';
  }
}

//! Collects the read vs. write period durations and number of events in each
//! period for an application.
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
    auto myRDStream = openFile(std::string("read-durations-") +
                                   std::to_string(myCounter) + ".dat",
                               aOutputPath);
    auto myWDStream = openFile(std::string("write-durations-") +
                                   std::to_string(myCounter) + ".dat",
                               aOutputPath);
    auto myREStream = openFile(std::string("read-events-") +
                                   std::to_string(myCounter) + ".dat",
                               aOutputPath);
    auto myWEStream = openFile(std::string("write-events-") +
                                   std::to_string(myCounter) + ".dat",
                               aOutputPath);
    for (const auto& myValue : myPeriod.second.theReadDurations) {
      *myRDStream << myValue << '\n';
    }
    for (const auto& myValue : myPeriod.second.theWriteDurations) {
      *myWDStream << myValue << '\n';
    }
    for (const auto& myValue : myPeriod.second.theReadEvents) {
      *myREStream << myValue << '\n';
    }
    for (const auto& myValue : myPeriod.second.theWriteEvents) {
      *myWEStream << myValue << '\n';
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

  auto myStream = openFile("num-invocations.dat", aOutputPath);
  for (const auto& elem : myNumInvocations) {
    *myStream << elem.first << ',' << elem.second << '\n';
  }
}

int main(int argc, char* argv[]) {
  us::GlogRaii myGlogRaii(argv[0]);

  std::string myDatasetFilename;
  std::string myOutputDir;
  std::string myAnalysis;
  double      mySessionDuration;

  po::options_description myDesc("Allowed options");
  // clang-format off
  myDesc.add_options()
    ("help,h", "produce help message")
    ("version,v", "print version and quit")
    ("input-dataset",
     po::value<std::string>(&myDatasetFilename)->required(),
     "Input dataset file name.")
    ("output-dir",
     po::value<std::string>(&myOutputDir)->required(),
     "Output directory.")
    ("analysis",
     po::value<std::string>(&myAnalysis)->default_value("num-invocations"),
     "Type of analysis, one of: {read-write-periods, num-invocations, lifecycles}.")
    ("singletons",
     "Also include functions called only once (used with lifecycles analysis).")
    ("session-duration",
     po::value<double>(&mySessionDuration)->default_value(60),
     "Duration of a session, in minutes, after the last event (used with lifecycles analysis).")
    ;
  // clang-format on

  try {
    po::variables_map myVarMap;
    po::store(po::parse_command_line(argc, argv, myDesc), myVarMap);
    po::notify(myVarMap);

    if (myVarMap.count("help")) {
      std::cout << myDesc << std::endl;
      return EXIT_SUCCESS;
    }

    if (myVarMap.count("version")) {
      std::cout << us::version() << std::endl;
      return EXIT_SUCCESS;
    }

    if (not boost::filesystem::is_directory(myOutputDir)) {
      throw std::runtime_error("Output directory does not exist: " +
                               myOutputDir);
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

    } else if (myAnalysis == "lifecycles") {
      Lifecycles myLifecycles;
      lifecycles(myDataset, myLifecycles, mySessionDuration);
      saveLifecycles(
          myLifecycles, myOutputDir, myVarMap.count("singletons") > 0);

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
