#pragma once

#include <boost/filesystem.hpp>

#include <stdexcept>

namespace uiiit {
namespace support {

//! \throw std::runtime_error if any of aFiles does not exist or is not regular.
template <class CONTAINER>
void checkFiles(const CONTAINER& aFiles) {
  for (const auto& myFile : aFiles) {
    if (myFile.empty()) {
      throw std::runtime_error("Empty file name");
    }
    if (not boost::filesystem::exists(myFile)) {
      throw std::runtime_error("File does not exist: " + myFile);
    }
    if (not boost::filesystem::is_regular(myFile)) {
      throw std::runtime_error("File is not regular: " + myFile);
    }
    if (boost::filesystem::is_empty(myFile)) {
      throw std::runtime_error("File is empty: " + myFile);
    }
  }
}

} // namespace support
} // namespace uiiit
