/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2019 Claudio Cicconetti https://ccicconetti.github.io/

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

#include "histogram.h"

#include "Support/fit.h"

#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace uiiit {
namespace support {

Histogram::Histogram(const Real           aLower,
                     const Real           aBinSpan,
                     const size_t         aNumBins,
                     const OverflowPolicy aPolicy)
    : theLower(aLower)
    , theBinSpan(aBinSpan)
    , thePolicy(aPolicy)
    , theStats(aNumBins)
    , theUnderflow()
    , theOverflow() {
  assert(aNumBins < std::numeric_limits<ssize_t>::max());
  if (aNumBins == 0) {
    throw std::runtime_error("Cannot make a histogram with no bins");
  }
  if (aBinSpan <= 0) {
    throw std::runtime_error(
        "Cannot make a histogram with non-positive bin span");
  }
}

void Histogram::operator()(const Real aValue, const Real aWeight) {
  const auto   myBin = binNdx(aValue);
  SummaryStat* bin   = nullptr;
  if (myBin < 0 or myBin >= static_cast<ssize_t>(theStats.size())) {
    if (thePolicy == THROW) {
      throw std::runtime_error("Cannot add value " + std::to_string(aValue) +
                               ": outside of domain " + domain());
    } else if (thePolicy == KEEP) {
      if (myBin < 0) {
        bin = &theUnderflow;
      } else {
        assert(myBin >= static_cast<ssize_t>(theStats.size()));
        bin = &theOverflow;
      }
    } else {
      assert(thePolicy == IGNORE);
      assert(bin == nullptr);
      return; // short-cut to the end of the call
    }
  } else {
    bin = &theStats.at(myBin);
  }

  assert(bin != nullptr);
  (*bin)(aWeight);
}

SummaryStat& Histogram::stat(const Real aValue) {
  const auto myBinNdx = binNdx(aValue);
  throwIfOutside(myBinNdx);
  return theStats.at(myBinNdx);
}

Histogram::Real Histogram::closestMean(const Real aValue,
                                       const bool aMinZero) const {
  const auto myBinNdx = binNdx(aValue);

  // 0. throw if the value given is outside the domain
  // this is the only case in which this method may throw
  throwIfOutside(myBinNdx);

  // 1. the exact bin has got values: use them to return mean
  if (not theStats.at(myBinNdx).empty()) {
    return theStats.at(myBinNdx).mean();
  }

  // 2. find the lower/upper bound values, starting from the exact bin and
  // moving backward/upward, respectively

  // 2a. search for the lower value first
  Real myLowerY;
  Real myLowerX;
  auto myLowerNdx = myBinNdx - 1; // note myBinNdx is _signed_
  for (; myLowerNdx >= 0; myLowerNdx--) {
    if (not theStats.at(myLowerNdx).empty()) {
      myLowerX = binValue(myLowerNdx);
      myLowerY = theStats.at(myLowerNdx).mean();
      break;
    }
  }

  // 2b. then search for the uppwer value
  Real myUpperY;
  Real myUpperX;
  auto myUpperNdx = myBinNdx + 1;
  for (; myUpperNdx < static_cast<ssize_t>(theStats.size()); myUpperNdx++) {
    if (not theStats.at(myUpperNdx).empty()) {
      myUpperX = binValue(myUpperNdx);
      myUpperY = theStats.at(myUpperNdx).mean();
      break;
    }
  }

  // 3. now return depending on what we have found:
  // - no lower, no upper:              return 0
  // - no lower,    upper, no aMinZero: return upper
  // - no lower,    upper, aMinZero:    return interpolation with 0,0
  // -    lower, no upper:              return low
  // -    lower,    upper:              return interpolation
  const auto myLowerFound = myLowerNdx >= 0;
  const auto myUpperFound = myUpperNdx < static_cast<ssize_t>(theStats.size());
  if (not myLowerFound and not myUpperFound) {
    return 0;
  } else if (not myLowerFound and myUpperFound) {
    if (aMinZero) {
      return support::extrapolate<Real>(0, 0, myUpperX, myUpperY, aValue);
    } else {
      return myUpperY;
    }
  } else if (myLowerFound and not myUpperFound) {
    return myLowerY;
  }
  assert(myLowerFound and myUpperFound);
  return support::extrapolate<Real>(
      myLowerX, myLowerY, myUpperX, myUpperY, aValue);
}

SummaryStat& Histogram::underflow() {
  if (thePolicy == KEEP) {
    return theUnderflow;
  }
  throw std::runtime_error("Underflow statistics not available");
}

SummaryStat& Histogram::overflow() {
  if (thePolicy == KEEP) {
    return theOverflow;
  }
  throw std::runtime_error("Overflow statistics not available");
}

void Histogram::print(std::ostream& aStream) {
  struct Printer {
    Printer(std::ostream& aStream, const Real aLower, const Real aBinSpan)
        : theStream(aStream)
        , theLower(aLower)
        , theBinSpan(aBinSpan) {
    }
    void operator()(const std::string& aHeader, SummaryStat& aStat) {
      theStream << aHeader << ' ';
      common(aStat);
    }
    void operator()(const ssize_t myBin, SummaryStat& aStat) {
      theStream << "bin#" << myBin << ' ' << (theLower + myBin * theBinSpan)
                << ".." << (theLower + (1 + myBin) * theBinSpan) << ": ";
      common(aStat);
    }

   private:
    void common(SummaryStat& aStat) {
      theStream << aStat.count() << ' ' << aStat.min() << ' ' << aStat.mean()
                << ' ' << aStat.max() << ' ' << aStat.stddev() << '\n';
    }
    std::ostream& theStream;
    const Real    theLower;
    const Real    theBinSpan;
  };

  Printer myPrinter(aStream, theLower, theBinSpan);
  if (thePolicy == KEEP) {
    myPrinter("underflow:", theUnderflow);
  }
  for (size_t i = 0; i < theStats.size(); i++) {
    myPrinter(i, theStats.at(i));
  }
  if (thePolicy == KEEP) {
    myPrinter("overflow:", theOverflow);
  }
}

ssize_t Histogram::binNdx(const Real aValue) const noexcept {
  return floor((aValue - theLower) / theBinSpan);
}

void Histogram::throwIfOutside(const ssize_t aBinNdx) const {
  if (aBinNdx < 0 or aBinNdx >= static_cast<ssize_t>(theStats.size())) {
    throw std::runtime_error("Value outside of the domain: " + domain());
  }
}

Histogram::Real Histogram::binValue(const ssize_t aBinNdx) const noexcept {
  return theLower + (static_cast<Real>(0.5) + aBinNdx) * theBinSpan;
}

std::string Histogram::domain() const {
  return "[" + std::to_string(theLower) + "," +
         std::to_string(theLower + theStats.size() * theBinSpan) + "]";
}

} // namespace support
} // namespace uiiit
