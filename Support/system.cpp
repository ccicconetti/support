#include "system.h"

#include <glog/logging.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace uiiit {
namespace support {

std::string cmd(const std::string& aCmd) {
  auto myPipe = popen((aCmd + " 2> /dev/null").c_str(), "r");
  if (!myPipe) {
    throw std::runtime_error("error executing command '" + aCmd +
                             "': " + strerror(errno));
  }

  char        myBuf[1 << 10];
  std::string ret;
  while (not feof(myPipe)) {
    if (fgets(myBuf, sizeof(myBuf) / sizeof(char), myPipe) != nullptr) {
      ret += myBuf;
    }
  }
  pclose(myPipe);

  return ret;
}

System::System()
    : theHostName()
    , theCpuName() {
}

System& System::instance() {
  static System mySingleton;
  return mySingleton;
}

std::string System::hostName() noexcept {
  if (not theHostName.empty()) {
    return theHostName;
  }

  try {
    const auto myOut = cmd("hostname -f");
    const auto myPos = myOut.find('\n');
    theHostName      = myOut.substr(0, myPos);

  } catch (std::exception& aErr) {
    LOG(ERROR) << "Cannot infer host name: " << aErr.what();
    theHostName = "unknown";

  } catch (...) {
    LOG(ERROR) << "Cannot infer host name";
    theHostName = "unknown";
  }

  return theHostName;
}

std::string System::cpuName() noexcept {
  if (not theCpuName.empty()) {
    return theCpuName;
  }

  theCpuName = cpuNameLinux();

  if (not theCpuName.empty()) {
    return theCpuName;
  } else {
    theCpuName = cpuNameMac();
  }

  if (not theCpuName.empty()) {
    return theCpuName;
  } else {
    LOG(ERROR) << "Cannot infer CPU name";
    theCpuName = "unknown";
  }

  return theCpuName;
}

std::string System::cpuNameLinux() noexcept {
  std::string ret;

  try {
    std::stringstream myStream;
    myStream << cmd("lscpu");

    std::string myLine;
    while (std::getline(myStream, myLine)) {
      if (myLine.empty()) {
        break;
      }

      auto myPos = myLine.find(':');
      if (myPos == std::string::npos or
          myLine.substr(0, myPos) != "Model name") {
        continue;
      }

      if (myPos == (myLine.size() - 1)) {
        continue;
      }
      myPos++;
      for (; myPos < myLine.size(); myPos++) {
        if (myLine[myPos] != ' ') {
          break;
        }
      }
      return myLine.substr(myPos, std::string::npos);
    }
    // if we reach this point we have not found the CPU model name

  } catch (...) {
    // silent ignore
  }

  return ret;
}

std::string System::cpuNameMac() noexcept {
  std::string ret;
  try {
    const auto myOut = cmd("sysctl -n machdep.cpu.brand_string");
    const auto myPos = myOut.find('\n');
    return myOut.substr(0, myPos);
  } catch (...) {
    // silent ignore
  }
  return ret;
}

} // namespace support
} // namespace uiiit
