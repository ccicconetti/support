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
