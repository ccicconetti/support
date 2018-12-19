#include "thrower.h"

#include <stdexcept>

#include <cassert>

namespace uiiit {
namespace support {

Thrower::Thrower(const std::string& aHeader)
    : theErr()
    , theNum(0)
    , theChecked(false) {
  theErr << aHeader;
}

Thrower::~Thrower() {
  assert(theChecked);
}

void Thrower::check() {
  theChecked = true;
  if (theNum > 0) {
    throw std::runtime_error(theErr.str());
  }
}

void Thrower::operator()(const bool aError, const std::string& aMessage) {
  theChecked = false;
  if (not aError) {
    return;
  }

  if (theNum > 0) {
    theErr << ", ";
  }
  theErr << aMessage;
  theNum++;
}

} // namespace support
} // namespace uiiit
