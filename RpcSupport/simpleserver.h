/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2019 Claudio Cicconetti https://ccicconetti.github.io/

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

#include "Support/macros.h"

#include <memory>
#include <string>

namespace grpc {
class Service;
class Server;
} // namespace grpc

namespace uiiit {
namespace rpc {

/**
 * Single-thread non-streaming server runner.
 */
class SimpleServer
{
 public:
  NONCOPYABLE_NONMOVABLE(SimpleServer)

  explicit SimpleServer(const std::string& aServerEndpoint);

  virtual ~SimpleServer();

  //! \return true if running.
  bool running() const;

  /**
   * Start the server.
   *
   * \param aBlocking true if this call blocks until server shut down, which
   * must be initiated by another thread.
   */
  void run(const bool aBlocking);

 private:
  virtual grpc::Service& service() = 0;

 private:
  const std::string             theServerEndpoint;
  std::unique_ptr<grpc::Server> theServer;
};

} // end namespace rpc
} // end namespace uiiit
