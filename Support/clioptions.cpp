/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2018 Claudio Cicconetti <https://about.me/ccicconetti>

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "clioptions.h"

#include "Support/versionutils.h"

#include <iostream>

namespace uiiit {
namespace support {

CliOptions::CliOptions(int                                          aArgc,
                       char**                                       aArgv,
                       boost::program_options::options_description& aDesc)
    : theVarMap()
    , theArgc(aArgc)
    , theArgv(aArgv)
    , theDesc(aDesc) {
  // clang-format off
  theDesc.add_options()
    ("help,h", "produce help message")
    ("version,v", "print program version")
    ;
  // clang-format on
}

void CliOptions::parse() {
  boost::program_options::store(
      boost::program_options::parse_command_line(theArgc, theArgv, theDesc),
      theVarMap);
  boost::program_options::notify(theVarMap);

  if (theVarMap.count("help") > 0) {
    std::cout << theDesc << std::endl;
    throw CliExit();
  }

  if (theVarMap.count("version") > 0) {
    std::cout << version() << std::endl;
    throw CliExit();
  }
}

const boost::program_options::variables_map& CliOptions::varMap() noexcept {
  return theVarMap;
}

} // namespace support
} // namespace uiiit
