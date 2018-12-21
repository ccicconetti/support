/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2018 Claudio Cicconetti <https://about.me/ccicconetti>

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

#include <condition_variable>
#include <list>
#include <mutex>
#include <stdexcept>

namespace uiiit {
namespace support {

//! Thrown when the queue is closed when waiting on a pop().
struct QueueClosed : public std::runtime_error {
  explicit QueueClosed();
};

/**
 * Thread-safe infinite queue of elements/messages.
 */
template <class T>
class Queue
{
 public:
  explicit Queue() noexcept
      : theMutex()
      , theEmptyCv()
      , theList()
      , theClosed(false) {
  }

  //! Add one element to the queue. Unblocks a thread waiting on pop(), if any.
  void push(const T& aElem) {
    const std::lock_guard<std::mutex> myLock(theMutex);
    theList.push_back(aElem);
    theEmptyCv.notify_one();
  }

  //! Add one element to the queue. Unblocks a thread waiting on pop(), if any.
  void push(T&& aElem) {
    const std::lock_guard<std::mutex> myLock(theMutex);
    theList.emplace_back(std::move(aElem));
    theEmptyCv.notify_one();
  }

  /**
   * This method blocks until there is at least one element in the queue.
   *
   * \return The earliest element add to the queue.
   *
   * \throw QueueClosed if the queue is closed while waiting.
   */
  T pop() {
    std::unique_lock<std::mutex> myLock(theMutex);
    theEmptyCv.wait(myLock,
                    [this]() { return theClosed or not theList.empty(); });

    if (theClosed) {
      throw QueueClosed();
    }

    T myRet = std::move(theList.front());
    theList.pop_front();
    return myRet;
  }

  /**
   * Close this queue. All threads blocked on pop() are awoken and they are
   * thrown an exception of type QueueClosed.
   */
  void close() noexcept {
    const std::lock_guard<std::mutex> myLock(theMutex);
    theClosed = true;
    theEmptyCv.notify_all();
  }

  //! \return the number of elements queued.
  size_t size() const noexcept {
    const std::lock_guard<std::mutex> myLock(theMutex);
    return theList.size();
  }

 private:
  mutable std::mutex      theMutex;
  std::condition_variable theEmptyCv;
  std::list<T>            theList;
  bool                    theClosed;
};

} // namespace support
} // namespace uiiit
