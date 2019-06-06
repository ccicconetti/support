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

#pragma once

#include <functional>
#include <sstream>
#include <string>

template <class T>
std::string toString(const T& aObject) noexcept {
  std::string myErr;
  try {
    std::stringstream myStream;
    myStream << aObject;
    return myStream.str();
  } catch (const std::exception& e) {
    myErr = std::string("Exception caught: ") + e.what();
  } catch (...) {
    myErr = "Unknown exception caught";
  }
  return myErr;
}

template <class CONTAINER>
std::string toString(
    const CONTAINER&   aContainer,
    const std::string& aSep,
    const std::function<std::string(typename CONTAINER::value_type)>& aMapping =
        [](const typename CONTAINER::value_type& aValue) { return aValue; }) {
  std::string myRet;
  auto        myFirst = true;
  for (const auto& myElem : aContainer) {
    if (myFirst) {
      myFirst = false;
    } else {
      myRet += aSep;
    }
    myRet += aMapping(myElem);
  }
  return myRet;
}
