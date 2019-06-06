/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/support/
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

#include "conf.h"

#include <glog/logging.h>

#include <algorithm>
#include <cassert>

namespace uiiit {
namespace support {

Conf::Conf(const std::string& aList) {
  size_t myPos = 0;
  while (myPos < aList.size()) {
    // find next key
    auto myNewPos = aList.find('=', myPos);
    if (myNewPos == std::string::npos) {
      throw InvalidConf(aList);
    }

    const auto myKey = aList.substr(myPos, myNewPos - myPos);
    if (myKey.empty() or myKey.front() == ',') {
      throw InvalidConf(aList);
    }
    myPos = myNewPos + 1;

    // find next value
    myNewPos           = aList.find(',', myPos);
    const auto myValue = (myNewPos == std::string::npos) ?
                             aList.substr(myPos) :
                             aList.substr(myPos, myNewPos - myPos);
    if (myValue.empty() or myValue.front() == '=') {
      throw InvalidConf(aList);
    }
    myPos = (myNewPos == std::string::npos) ? aList.size() : (myNewPos + 1);

    // add pair to this object
    const auto it = insert(std::make_pair(myKey, myValue));
    LOG_IF(WARNING, not it.second)
        << "Duplicate key '" << myKey << "' found in configuration: using "
        << it.first->second << ", ignoring " << myValue;
  }
}

std::string Conf::operator()(const std::string& aKey) const {
  const auto it = find(aKey);
  if (it == end()) {
    return std::string();
  }
  return it->second;
}

double Conf::getDouble(const std::string& aKey) const {
  return getArithmetic<double, InvalidDouble>(aKey);
}

int Conf::getInt(const std::string& aKey) const {
  return getArithmetic<int, InvalidInt>(aKey);
}

unsigned int Conf::getUint(const std::string& aKey) const {
  const auto myIntValue = getInt(aKey);
  if (myIntValue < 0) {
    throw InvalidLogic(aKey, (*this)(aKey), "cannot be < 0");
  }
  return static_cast<unsigned int>(myIntValue);
}

bool Conf::getBool(const std::string& aKey) const {
  auto myValueString = (*this)(aKey);
  if (myValueString.empty()) {
    throw NoValue(aKey);
  }
  std::transform(myValueString.begin(),
                 myValueString.end(),
                 myValueString.begin(),
                 ::tolower);
  if (myValueString == "true" or myValueString == "yes" or
      myValueString == "1") {
    return true;
  } else if (myValueString == "false" or myValueString == "no" or
             myValueString == "0") {
    return false;
  }

  throw InvalidBool(myValueString);
}

std::set<std::string> Conf::keys() const {
  std::set<std::string> myRet;
  for (const auto& myPair : *this) {
    myRet.insert(myPair.first);
  }
  return myRet;
}

std::set<std::string> Conf::values() const {
  std::set<std::string> myRet;
  for (const auto& myPair : *this) {
    myRet.insert(myPair.second);
  }
  return myRet;
}

} // namespace support
} // namespace uiiit
