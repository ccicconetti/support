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

#pragma once

#include <boost/program_options.hpp>

#include <stdexcept>

namespace uiiit {
namespace support {

//! Raised to trigger a clean quit of the application after parsing CLI options.
struct CliExit final : public std::runtime_error {
  explicit CliExit()
      : std::runtime_error("") {
  }
};

/**
 * Basic wrapper to boost::program_options functions to parse command-line
 * arguments. Must be derived.
 *
 * Add --help and --version default arguments.
 */
class CliOptions
{
 public:
  virtual ~CliOptions() {
  }

  //! \return the map of variables parsed from CLI.
  const boost::program_options::variables_map& varMap() noexcept;

 protected:
  CliOptions(int                                          argc,
             char**                                       argv,
             boost::program_options::options_description& aDesc);

  /**
   * Perform actual parsing of the command-line arguments.
   * Must be called by the derived classes.
   */
  void parse();

 protected:
  boost::program_options::variables_map        theVarMap;
  int                                          theArgc;
  char**                                       theArgv;
  boost::program_options::options_description& theDesc;
};

} // namespace support
} // namespace uiiit
