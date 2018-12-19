#pragma once

#include <stdexcept>

namespace uiiit {
namespace support {

struct ZeroWindow final : public std::runtime_error {
  explicit ZeroWindow()
      : std::runtime_error("Zero window") {
  }
};

struct EmptyWindow final : public std::runtime_error {
  explicit EmptyWindow()
      : std::runtime_error("Empty window") {
  }
};

} // namespace support
} // namespace uiiit
