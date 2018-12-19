#pragma once

#include <boost/uuid/uuid.hpp>

#include <string>

namespace uiiit {
namespace support {

class Uuid final
{
 public:
  //! Create a random unique identifier.
  explicit Uuid();
  //! Load the unique identifier from aValue.
  explicit Uuid(const std::string& aValue);

  //! Convert the UUID to a string.
  std::string toString();

  bool operator==(const Uuid& aOther) const;
  bool operator!=(const Uuid& aOther) const;

 private:
  static boost::uuids::uuid get();

 private:
  boost::uuids::uuid theUuid;
};

} // namespace support
} // namespace uiiit
