#pragma once

namespace uiiit {
namespace support {

class GlogRaii final
{
 public:
  explicit GlogRaii(const char* argv0);
  ~GlogRaii();
};

} // namespace support
} // namespace uiiit
