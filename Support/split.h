#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <vector>

namespace uiiit {
namespace support {

template <class CONTAINER>
CONTAINER split(const std::string& aInput, const std::string& aDelimiters) {
  std::vector<std::string> myTokens;
  boost::split(myTokens,
               aInput,
               boost::is_any_of(aDelimiters),
               boost::token_compress_on);

  CONTAINER ret;
  for (const auto& myToken : myTokens) {
    if (myToken.empty()) {
      continue;
    }
    ret.insert(ret.end(),
               boost::lexical_cast<typename CONTAINER::value_type>(myToken));
  }

  return ret;
}

} // namespace support
} // namespace uiiit
