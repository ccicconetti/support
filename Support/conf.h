#pragma once

#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

namespace uiiit {
namespace support {

struct ConfException : public std::runtime_error {
  explicit ConfException(const std::string& aErr)
      : std::runtime_error(aErr) {
  }
};

struct InvalidConf final : public ConfException {
  explicit InvalidConf(const std::string& aConf)
      : ConfException("Invalid configuration: " + aConf) {
  }
};

struct NoValue final : public ConfException {
  explicit NoValue(const std::string& aKey)
      : ConfException("No value found for key: " + aKey) {
  }
};

struct InvalidType : public ConfException {
 protected:
  explicit InvalidType(const std::string& aValue, const std::string& aType)
      : ConfException("Invalid type when converting " + aValue + " as " +
                      aType) {
  }
};

struct InvalidDouble : public InvalidType {
  explicit InvalidDouble(const std::string& aValue)
      : InvalidType(aValue, "double") {
  }
};

struct InvalidInt : public InvalidType {
  explicit InvalidInt(const std::string& aValue)
      : InvalidType(aValue, "int") {
  }
};

struct InvalidBool : public InvalidType {
  explicit InvalidBool(const std::string& aValue)
      : InvalidType(aValue, "bool") {
  }
};

struct InvalidLogic final : public std::runtime_error {
  explicit InvalidLogic(const std::string& aKey,
                        const std::string& aValue,
                        const std::string& aErr)
      : std::runtime_error("Invalid value '" + aValue + "' for key '" + aKey +
                           "': " + aErr) {
  }
};

class Conf : public std::map<std::string, std::string>
{
 public:
  //! A configuration with no keys/values.
  explicit Conf() {
  }

  /**
   * Create a configuration from a string consisting of comma-separated pairs
   * key1=value1,key2=value2,...
   *
   * \throw InvalidConf if there is a syntax error.
   */
  explicit Conf(const std::string& aList);

  //! \return The value associated to aKey or an empty string if not present.
  std::string operator()(const std::string& aKey) const;

  /**
   * \return The double value associated to aKey.
   *
   * \throw NoValue if there is no value associated to aKey.
   *
   * \throw InvalidDouble if the value cannot be converted to a double.
   */
  double getDouble(const std::string& aKey) const;

  /**
   * \return The integer value associated to aKey.
   *
   * \throw NoValue if there is no value associated to aKey.
   *
   * \throw InvalidInt if the value cannot be converted to an integer.
   */
  int getInt(const std::string& aKey) const;

  /**
   * \return The unsigned integer value associated to aKey.
   *
   * \throw NoValue if there is no value associated to aKey.
   *
   * \throw InvalidInt if the value cannot be converted to an integer.
   *
   * \throw InvalidLogic if the value converted to an integer is negative.
   */
  unsigned int getUint(const std::string& aKey) const;

  /**
   * \return the Boolean value associated to aKey, ie. true if aKey is "true" or
   * "yes" (both case insensitive) or "1", and false if aKey is "false" or "no"
   * (both case insensitive) or "0".
   *
   * \throw NoValue if there is no value associated to aKey.
   *
   * \throw InvalidBool if the value cannot be converted to a bool
   */
  bool getBool(const std::string& aKey) const;

  //! \return all the keys.
  std::set<std::string> keys() const;

  //! \return all the values..
  std::set<std::string> values() const;

 private:
  template <class TYPE, class EXCEPTION>
  TYPE getArithmetic(const std::string& aKey) const {
    const auto myValue = (*this)(aKey);
    if (myValue.empty()) {
      throw NoValue(aKey);
    }

    std::stringstream myStream;
    myStream << myValue;

    TYPE myRet;
    if (not(myStream >> myRet)) {
      throw EXCEPTION(myValue);
    }

    return myRet;
  }
};

} // namespace support
} // namespace uiiit
