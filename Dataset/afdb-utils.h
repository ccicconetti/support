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

#pragma once

#include "Support/glograii.h"
#include "Support/split.h"

#include <glog/logging.h>

#include <array>
#include <cassert>
#include <deque>
#include <exception>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>

namespace uiiit {
namespace dataset {

/**
 * @brief A row from the Azure function dataset:
 *
 * https://github.com/Azure/AzurePublicDataset/blob/master/AzureFunctionsBlobDataset2020.md
 */
struct Row {
  explicit Row(const std::string& aRow);

  std::string key() const {
    return theUser + "," + theApp;
  }

  double      theTimestamp;   // in ms
  std::string theRegion;      // unique ID for the region
  std::string theUser;        // unique ID for the user
  std::string theApp;         // unique ID for the app
  std::string theFunction;    // unique ID for the invocation
  std::string theBlob;        // unique ID for the BLOB accessed
  std::string theBlobType;    // BLOB type
  std::string theBlobVersion; // BLOB version
  std::size_t theBlobSize;    // in bytes
  bool        theWrite;       // true: write access; false: read access
};

/**
 * @brief Load a dataset in memory.
 *
 * @param aStream The stream containing the dataset.
 * @param aWithHeader True if the header is present
 * @return std::deque<Row> The in-memory dataset.
 */
std::deque<Row> loadDataset(std::istream& aStream, const bool aWithHeader);

/**
 * @brief How we assume functions will be invoked.
 */
enum class ExecMode : unsigned int {
  AlwaysMu     = 0, //!< microservices only
  AlwaysLambda = 1, //!< stateless functions only
  BestNext     = 2, //!< decide based on the next invocation
  Size         = 3,
};

std::string toString(const ExecMode aExecMode);

const std::array<ExecMode, static_cast<unsigned int>(ExecMode::Size)>&
allExecModes();

struct CostModel {
  double theCostExecMu        = 0;
  double theCostExecLambda    = 0;
  double theCostReadLambda    = 0;
  double theCostWriteLambda   = 0;
  double theCostWarmMu        = 0;
  double theCostWarmLambda    = 0;
  double theCostMigrateMu     = 0;
  double theCostMigrateLambda = 0;

  std::string                            toString() const;
  static const std::vector<std::string>& explain();
};

struct CostOutput {
  enum class Type : unsigned int {
    Microservice = 0,
    Stateless    = 1,
  };

  // output variables
  double      theDuration       = 0;
  std::size_t theNumInvocations = 0;
  std::array<double, static_cast<unsigned int>(ExecMode::Size)> theCosts;

  std::size_t theBestNextNumMu     = 0;
  std::size_t theBestNextNumLambda = 0;
  double      theBestNextDurMu     = 0;
  double      theBestNextDurLambda = 0;

  // internal variables
  Type theBestNextLastType = Type::Stateless;

  // durations of the alternating periods with best-next, starting with the
  // default value of theBestNextLastType
  std::deque<double> theBestNextPeriods;

  std::string                            toString() const;
  static const std::vector<std::string>& explain();
};

/**
 * @brief A subset of the dataset only including the timestamps and the
 * read/write flag, grouped by key and sorted in chronological order.
 */
using TimestampDataset =
    std::unordered_map<std::string, std::deque<std::tuple<double, bool>>>;

/**
 * @brief Convert a raw dataset to a timestamp dataset.
 *
 * @param aDataset The raw dataset.
 * @return TimestampDataset
 */
TimestampDataset toTimestampDataset(const std::deque<Row>& aDataset);

/**
 * @brief Load a timestamp dataset from file.
 *
 * @param aFilename The name of the file to load the dataset from.
 * @return TimestampDataset
 */
TimestampDataset loadTimestampDataset(const std::string& aFilename);

/**
 * @brief Load a timestamp dataset from file.
 *
 * @param aFilename The name of the file to load the dataset from.
 * @return TimestampDataset
 */
void saveTimestampDataset(const TimestampDataset& aDataset,
                          const std::string&      aFilename);

/**
 * @brief Compute the execution cost of function invocation with all modes.
 *
 * @param aDataset The input dataset.
 * @param aCostModel The cost model.
 * @param aSaveBnPeriods If true also save the alternating best-next periods.
 * @return The performance metrics, one per key.
 */
std::unordered_map<std::string, CostOutput>
cost(const TimestampDataset& aDataset,
     const CostModel&        aCostModel,
     const bool              aSaveBnPeriods);

} // namespace dataset
} // namespace uiiit