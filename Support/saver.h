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

#include "Support/chrono.h"
#include "Support/macros.h"

#include <fstream>
#include <memory>
#include <mutex>
#include <stdexcept>

namespace uiiit {
namespace support {

//! A thread-safe file serialized.
class Saver
{
 public:
  NONCOPYABLE_NONMOVABLE(Saver);

  explicit Saver(const std::string& aFilename,
                 const bool         aTimestamp,
                 const bool         aFlush)
      : theMutex()
      , theFile(aFilename.empty() ? nullptr : new std::ofstream(aFilename))
      , theChrono(aTimestamp)
      , theFlush(aFlush) {
    if (not aFilename.empty() and not theFile->is_open()) {
      throw std::runtime_error("Could not open '" + aFilename + "'");
    }
  }

  template <typename T>
  T adder(T v) {
    return v;
  }

  template <typename T, typename... Args>
  T adder(T first, Args... args) {
    return first + adder(args...);
  }

  template <class TYPE>
  void operator()(const TYPE& aValue) const {
    if (not theFile) {
      return;
    }

    const std::lock_guard<std::mutex> myLock(theMutex);
    timestamp();
    *theFile << aValue << '\n';
    flush();
  }

  template <class TYPE1, class TYPE2>
  void operator()(const TYPE1& aValue1, const TYPE2& aValue2) const {
    if (not theFile) {
      return;
    }

    const std::lock_guard<std::mutex> myLock(theMutex);
    timestamp();
    *theFile << aValue1 << ' ' << aValue2 << '\n';
    flush();
  }

  template <class TYPE1, class TYPE2, class TYPE3>
  void operator()(const TYPE1& aValue1,
                  const TYPE2& aValue2,
                  const TYPE3& aValue3) const {
    if (not theFile) {
      return;
    }

    const std::lock_guard<std::mutex> myLock(theMutex);
    timestamp();
    *theFile << aValue1 << ' ' << aValue2 << ' ' << aValue3 << '\n';
    flush();
  }

  template <class TYPE1, class TYPE2, class TYPE3, class TYPE4>
  void operator()(const TYPE1& aValue1,
                  const TYPE2& aValue2,
                  const TYPE3& aValue3,
                  const TYPE4& aValue4) const {
    if (not theFile) {
      return;
    }

    const std::lock_guard<std::mutex> myLock(theMutex);
    timestamp();
    *theFile << aValue1 << ' ' << aValue2 << ' ' << aValue3 << ' ' << aValue4
             << '\n';
    flush();
  }

  template <class TYPE1, class TYPE2, class TYPE3, class TYPE4, class TYPE5>
  void operator()(const TYPE1& aValue1,
                  const TYPE2& aValue2,
                  const TYPE3& aValue3,
                  const TYPE4& aValue4,
                  const TYPE5& aValue5) const {
    if (not theFile) {
      return;
    }

    const std::lock_guard<std::mutex> myLock(theMutex);
    timestamp();
    *theFile << aValue1 << ' ' << aValue2 << ' ' << aValue3 << ' ' << aValue4
             << ' ' << aValue5 << '\n';
    flush();
  }

  template <class TYPE1,
            class TYPE2,
            class TYPE3,
            class TYPE4,
            class TYPE5,
            class TYPE6>
  void operator()(const TYPE1& aValue1,
                  const TYPE2& aValue2,
                  const TYPE3& aValue3,
                  const TYPE4& aValue4,
                  const TYPE5& aValue5,
                  const TYPE6& aValue6) const {
    if (not theFile) {
      return;
    }

    const std::lock_guard<std::mutex> myLock(theMutex);
    timestamp();
    *theFile << aValue1 << ' ' << aValue2 << ' ' << aValue3 << ' ' << aValue4
             << ' ' << aValue5 << ' ' << aValue6 << '\n';
    flush();
  }

  template <class TYPE1,
            class TYPE2,
            class TYPE3,
            class TYPE4,
            class TYPE5,
            class TYPE6,
            class TYPE7>
  void operator()(const TYPE1& aValue1,
                  const TYPE2& aValue2,
                  const TYPE3& aValue3,
                  const TYPE4& aValue4,
                  const TYPE5& aValue5,
                  const TYPE6& aValue6,
                  const TYPE7& aValue7) const {
    if (not theFile) {
      return;
    }

    const std::lock_guard<std::mutex> myLock(theMutex);
    timestamp();
    *theFile << aValue1 << ' ' << aValue2 << ' ' << aValue3 << ' ' << aValue4
             << ' ' << aValue5 << ' ' << aValue6 << ' ' << aValue7 << '\n';
    flush();
  }

 private:
  void timestamp() const {
    if (theChrono) {
      *theFile << theChrono.time() << ' ';
    }
  }
  void flush() const {
    if (theFlush) {
      *theFile << std::flush;
    }
  }

 private:
  mutable std::mutex                   theMutex;
  const std::unique_ptr<std::ofstream> theFile;
  const support::Chrono                theChrono;
  const bool                           theFlush;
};

} // namespace support
} // namespace uiiit
