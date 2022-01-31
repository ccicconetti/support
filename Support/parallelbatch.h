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

#pragma once

#include "Support/queue.h"

#include <glog/logging.h>

#include <atomic>
#include <cassert>
#include <functional>
#include <string>
#include <thread>

namespace uiiit {
namespace support {

/**
 * Execute a batch of experiments with a pool of threads.
 */
template <class Parameter>
class ParallelBatch final
{
 public:
  using ParameterQueue = support::Queue<Parameter>;

  explicit ParallelBatch(
      const std::size_t                                  aNumThreads,
      ParameterQueue&                                    aParameterQueue,
      const std::function<void(Parameter&& aParameter)>& aExperimentFunctor);

  ~ParallelBatch();

  /**
   * @brief Wait for all the jobs to terminate.
   *
   * @return the list of exceptions thrown, if any.
   */
  std::list<std::string> wait();

 private:
  ParameterQueue&             theParameterQueue;
  const std::size_t           theNumExperiments;
  std::list<std::thread>      theThreads;
  support::Queue<std::string> theResults;
  std::atomic<bool>           theWait;
};

template <class Parameter>
ParallelBatch<Parameter>::ParallelBatch(
    const std::size_t                                  aNumThreads,
    ParameterQueue&                                    aParameterQueue,
    const std::function<void(Parameter&& aParameter)>& aExperimentFunctor)
    : theParameterQueue(aParameterQueue)
    , theNumExperiments(aParameterQueue.size())
    , theThreads()
    , theResults()
    , theWait(false) {
  for (std::size_t i = 0; i < aNumThreads; i++) {
    theThreads.emplace_back([i, &aExperimentFunctor, this]() {
      VLOG(1) << "worker #" << i << ": starting";
      while (true) {
        try {
          auto myParameter = theParameterQueue.pop();
          try {
            aExperimentFunctor(std::move(myParameter));
            theResults.push(std::string());
          } catch (const std::exception& aErr) {
            theResults.push("experiment #" + std::to_string(i) +
                            " exception thrown: " + aErr.what());
          } catch (...) {
            theResults.push("experiment #" + std::to_string(i) +
                            " unknown exception thrown");
          }
        } catch (const QueueClosed&) {
          VLOG(1) << "worker #" << i << ": terminating";
          break;
        }
      }
    });
  }
}

template <class Parameter>
ParallelBatch<Parameter>::~ParallelBatch() {
  // waiting for all results before terminating
  wait();
}

template <class Parameter>
std::list<std::string> ParallelBatch<Parameter>::wait() {
  if (theWait.exchange(true)) {
    // can't wait twice
    return std::list<std::string>();
  }
  std::list<std::string> ret;
  for (size_t i = 0; i < theNumExperiments; i++) {
    auto myResult = theResults.pop();
    if (not myResult.empty()) {
      // an exception was thrown by an experiment
      ret.emplace_back(std::move(myResult));
    }
  }
  theParameterQueue.close();
  for (auto& myThread : theThreads) {
    assert(myThread.joinable());
    myThread.join();
  }
  return ret;
}

} // namespace support
} // namespace uiiit
