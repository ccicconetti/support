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

#include "fileutils.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace uiiit {
namespace support {

std::ifstream openFile(const std::string& aFilepath) {
  std::ifstream myInFile(aFilepath, std::ios::binary);

  if (not myInFile) {
    throw std::runtime_error("Could not open file for reading: " + aFilepath);
  }
  return myInFile;
}

std::vector<char> readFile(const std::string& aFilepath) {
  auto myInFile = openFile(aFilepath);

  // go the end of the file to determine the size
  myInFile.seekg(0, std::ios::end);
  std::vector<char> myFileData(myInFile.tellg());

  // move back to the beginning and issue the read
  myInFile.seekg(0);
  myInFile.read(myFileData.data(), myFileData.size());

  return myFileData;
}

std::string readFileAsString(const std::string& aFilepath) {
  auto              myInFile = openFile(aFilepath);
  std::stringstream myStream;
  myStream << myInFile.rdbuf();
  return myStream.str();
}

} // namespace support
} // namespace uiiit
