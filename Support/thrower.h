#pragma once

#include <sstream>
#include <string>

namespace uiiit {
namespace support {

class Thrower final
{
 public:
  explicit Thrower(const std::string& aHeader);
  ~Thrower();
  void check();
  void operator()(const bool aError, const std::string& aMessage);

 private:
  std::stringstream theErr;
  unsigned int      theNum;
  bool              theChecked;
};

} // namespace support
} // namespace uiiit
