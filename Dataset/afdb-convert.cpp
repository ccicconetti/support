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
#include "Support/glograii.h"
#include "Support/versionutils.h"

#include <boost/program_options.hpp>

#include <cstdlib>
#include <fstream>
#include <string>

namespace po = boost::program_options;
namespace us = uiiit::support;
namespace ud = uiiit::dataset;

int main(int argc, char* argv[]) {
  us::GlogRaii myGlogRaii(argv[0]);

  std::string myInputRaw;
  std::string myOutputTimestamp;
  std::string myDumpTimestamp;

  po::options_description myDesc("Allowed options");
  // clang-format off
  myDesc.add_options()
    ("help,h", "produce help message")
    ("version,v", "print version and quit")
    ("input-raw",
     po::value<std::string>(&myInputRaw)->default_value(""),
     "Load a raw dataset from this file.")
    ("output-timestamp",
     po::value<std::string>(&myOutputTimestamp)->default_value(""),
     "Save the timestamp dataset to this file.")
    ("dump-timestamp",
     po::value<std::string>(&myDumpTimestamp)->default_value(""),
     "Dump the timestamp dataset in this file.")
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

    const auto myCommands =
        (not myOutputTimestamp.empty()) + (not myDumpTimestamp.empty());

    if (myCommands != 1) {
      std::cout << "Invalid command" << std::endl << myDesc << std::endl;
      return EXIT_FAILURE;
    }

    if (not myOutputTimestamp.empty()) {
      std::ifstream myRawStream(myInputRaw);
      if (not myRawStream) {
        throw std::runtime_error(
            "could not open raw dataset file for reading: " + myInputRaw);
      }
      ud::saveTimestampDataset(
          ud::toTimestampDataset(ud::loadDataset(myRawStream, false)),
          myOutputTimestamp);

    } else if (not myDumpTimestamp.empty()) {
      const auto myDataset = ud::loadTimestampDataset(myDumpTimestamp);
      for (const auto& myApp : myDataset) {
        std::cout << myApp.first << '\n';
        for (const auto& elem : myApp.second) {
          std::cout << '\t' << std::get<0>(elem) << '\t'
                    << (std::get<1>(elem) ? '1' : '0') << '\n';
        }
      }
    }

    return EXIT_SUCCESS;
  } catch (const std::exception& aErr) {
    std::cerr << "Exception caught: " << aErr.what() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }

  return EXIT_FAILURE;
}
