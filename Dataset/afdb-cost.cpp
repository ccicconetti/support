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
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>

#include <cstdlib>
#include <glog/logging.h>

#include <cassert>
#include <deque>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>

namespace po = boost::program_options;
namespace us = uiiit::support;
namespace ud = uiiit::dataset;

int main(int argc, char* argv[]) {
  us::GlogRaii myGlogRaii(argv[0]);

  std::string   myDatasetFilename;
  std::string   myOutputDir;
  std::string   myAnalysis;
  ud::CostModel myCostModel;

  po::options_description myDesc("Allowed options");
  // clang-format off
  myDesc.add_options()
    ("help,h", "Produce help message")
    ("version,v", "Print version and quit")
    ("explain", "Show the meaning of the columns in the output file")
    ("input-dataset",
     po::value<std::string>(&myDatasetFilename)->default_value(""),
     "Input dataset file name.")
    ("output-dir",
     po::value<std::string>(&myOutputDir)->default_value(""),
     "Output directory.")
    ("append", "Append to the output file instead of overwriting.")
    ("analysis",
     po::value<std::string>(&myAnalysis)->default_value("invocation-only"),
     "Type of analysis, one of: {invocation-only}.")
    ("cost-exec-mu",
     po::value<double>(&myCostModel.theCostExecMu)->default_value(1),
     "Cost of executing a single invocation as microservice.")
    ("cost-exec-lambda",
     po::value<double>(&myCostModel.theCostExecLambda)->default_value(10),
     "Cost of executing a single invocation as stateless function.")
    ("cost-read-lambda",
     po::value<double>(&myCostModel.theCostReadLambda)->default_value(1),
     "Cost of executing a read operation in a stateless function.")
    ("cost-write-lambda",
     po::value<double>(&myCostModel.theCostWriteLambda)->default_value(5),
     "Cost of executing a write operation in a stateless function.")
    ("cost-warm-mu",
     po::value<double>(&myCostModel.theCostWarmMu)->default_value(0.00015),
     "Cost of keeping a function warm as microservice.")
    ("cost-warm-lambda",
     po::value<double>(&myCostModel.theCostWarmLambda)->default_value(0),
     "Cost of keeping a function warm as stateless function.")
    ("cost-migrate-mu",
     po::value<double>(&myCostModel.theCostMigrateMu)->default_value(50),
     "Cost of migrating from stateless to microservice.")
    ("cost-migrate-lambda",
     po::value<double>(&myCostModel.theCostMigrateLambda)->default_value(50),
     "Cost of migrating from microservice to stateless")
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

    if (myVarMap.count("explain")) {
      std::cout << "#1\tapplication\n#2\tfunction\n";
      std::size_t myIndex = 3;
      for (const auto& myLabel : ud::CostModel::explain()) {
        std::cout << '#' << myIndex++ << '\t' << myLabel << '\n';
      }
      for (const auto& myLabel : ud::CostOutput::explain()) {
        std::cout << '#' << myIndex++ << '\t' << myLabel << '\n';
      }
      return EXIT_SUCCESS;
    }

    if (myDatasetFilename.empty()) {
      throw std::runtime_error("Empty input filename");
      return EXIT_FAILURE;
    }

    VLOG(1) << "reading from: " << myDatasetFilename;
    std::ifstream myDatasetStream(myDatasetFilename);
    if (not myDatasetStream) {
      throw std::runtime_error("could not open dataset file for reading: " +
                               myDatasetFilename);
    }
    auto myDataset = ud::loadDataset(myDatasetStream, false);

    VLOG(1) << "analyzing dataset";
    if (myAnalysis == "invocation-only") {
      std::ofstream mySummaryStream(
          (boost::filesystem::path(myOutputDir) /
           (boost::filesystem::path(myDatasetFilename).filename().string() +
            "-cost.dat"))
              .string(),
          myVarMap.count("append") ? std::ios::app : std::ios::trunc);
      const auto myCosts = cost(myDataset, myCostModel);
      for (const auto& myCost : myCosts) {
        mySummaryStream << myCost.first << ',' << myCostModel.toString() << ','
                        << myCost.second.toString() << '\n';
      }

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
