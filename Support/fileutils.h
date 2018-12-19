#pragma once

#include <string>
#include <vector>

namespace uiiit {
namespace support {

/**
 * Read a whole file into a vector of characters.
 */
std::vector<char> readFile(const std::string& aFilepath);

/**
 * Read a whole file into a string.
 */
std::string readFileAsString(const std::string& aFilepath);

} // namespace support
} // namespace uiiit
