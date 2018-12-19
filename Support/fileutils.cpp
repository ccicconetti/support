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
