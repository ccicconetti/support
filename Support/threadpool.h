#pragma once

#include <glog/logging.h>

#include <atomic>
#include <cassert>
#include <list>
#include <mutex>
#include <string>
#include <thread>

#include "Detail/caller.h"

namespace uiiit {
namespace support {

/**
 * Creates a thread pool, each with a given object associated that expose a
 * start() method.
 */
template <class OBJECT>
class ThreadPool final
{
 public:
  explicit ThreadPool();
  //! Wait for termination of all threads.
  ~ThreadPool();

  //! Add a new thread.
  void add(OBJECT&& aObject);

  //! Start all the threads.
  void start();

  //! Stop all the threads.
  void stop();

  /**
   * Wait for the termination of all threads.
   * It is safe to call this method multiple times.
   *
   * \return the list of exceptions thrown.
   */
  const std::list<std::string>& wait() noexcept;

 private:
  mutable std::mutex     theMutex;
  std::list<OBJECT>      theObjects;
  std::list<std::thread> theThreads;
  std::list<std::string> theExceptionsThrown;
  bool                   theStarted;
  std::atomic<bool>      theStopped;
};

template <class OBJECT>
ThreadPool<OBJECT>::ThreadPool()
    : theMutex()
    , theObjects()
    , theThreads()
    , theExceptionsThrown()
    , theStarted(false)
    , theStopped(false) {
}

template <class OBJECT>
ThreadPool<OBJECT>::~ThreadPool() {
  LOG_IF(WARNING, not theObjects.empty() and not theStarted)
      << "Terminating a thread pool that has not been started";
  wait();
}

template <class OBJECT>
void ThreadPool<OBJECT>::add(OBJECT&& aObject) {
  const std::lock_guard<std::mutex> myLock(theMutex);
  theObjects.emplace_back(std::move(aObject));
}

template <class OBJECT>
void ThreadPool<OBJECT>::start() {
  const std::lock_guard<std::mutex> myLock(theMutex);
  if (theStopped) {
    throw std::runtime_error("Cannot start a stopped thread pool");
  }
  if (theStarted) {
    throw std::runtime_error("Cannot restart a thread pool");
  }
  for (auto& myObject : theObjects) {
    theThreads.emplace_back(std::thread([this, &myObject]() {
      VLOG(1) << "Starting thread";
      std::string myException;
      try {
        detail::Caller<OBJECT>().call(myObject);
      } catch (const std::exception& aErr) {
        myException = aErr.what();
      } catch (...) {
        myException = "Unknown error";
      }
      const std::lock_guard<std::mutex> myLock(theMutex);
      if (not myException.empty()) {
        VLOG(1) << "Thread exited with exception: " << myException;
        theExceptionsThrown.emplace_back(std::move(myException));
      }
      VLOG(1) << "Terminating thread";
    }));
  }
  theStarted = true;
}

template <class OBJECT>
void ThreadPool<OBJECT>::stop() {
  const std::lock_guard<std::mutex> myLock(theMutex);
  if (not theStarted) {
    throw std::runtime_error("Cannot stop a thread pool not started");
  }
  for (auto& myObject : theObjects) {
    detail::Caller<OBJECT>().stop(myObject);
  }
}

template <class OBJECT>
const std::list<std::string>& ThreadPool<OBJECT>::wait() noexcept {
  if (theStopped.exchange(true)) {
    return theExceptionsThrown;
  }

  try {
    for (auto& myThread : theThreads) {
      myThread.join();
    }
  } catch (const std::exception& aErr) {
    LOG(ERROR) << "Exception caught while waiting for thread termination: "
               << aErr.what();
  } catch (...) {
    LOG(ERROR)
        << "Unknown exception caught while waiting for thread termination";
  }
  return theExceptionsThrown;
}

} // namespace support
} // namespace uiiit
