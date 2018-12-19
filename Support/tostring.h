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
