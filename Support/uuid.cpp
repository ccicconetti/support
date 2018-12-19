#include "uuid.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace uiiit {
namespace support {

Uuid::Uuid()
    : theUuid(get()) {
}

Uuid::Uuid(const std::string& aValue)
    : theUuid(boost::uuids::string_generator()(aValue)) {
}

std::string Uuid::toString() {
  return boost::uuids::to_string(theUuid);
}

bool Uuid::operator==(const Uuid& aOther) const {
  return theUuid == aOther.theUuid;
}

bool Uuid::operator!=(const Uuid& aOther) const {
  return not(*this == aOther);
}

boost::uuids::uuid Uuid::get() {
  static boost::uuids::random_generator myGenerator;
  return myGenerator();
}

} // namespace support
} // namespace uiiit
