/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/support/
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

#include "RpcSupport/utils.h"
#include "Support/fileutils.h"
#include "Support/macros.h"

#include <grpc++/grpc++.h>

#include <memory>
#include <string>

namespace uiiit {
namespace rpc {

/*
 * Base class for a synchronous gRPC client.
 */
template <class SERVER>
class SimpleClient
{
 public:
  NONCOPYABLE_NONMOVABLE(SimpleClient);

  /**
   * @brief Construct a new Simple Client object
   *
   * @param aServerEndpoint The end-point to which to connect for this service.
   * @param aSecure If true then use SSL/TLS authentication.
   */
  explicit SimpleClient(const std::string& aServerEndpoint,
                        const bool         aSecure = false)
      : theChannel(nullptr)
      , theStub(nullptr)
      , theServerEndpoint(aServerEndpoint) {
    theChannel = grpc::CreateChannel(
        aServerEndpoint,
        aSecure ? grpc::SslCredentials(sslClientCredOpts()) :
                  grpc::InsecureChannelCredentials());
    theStub = SERVER::NewStub(theChannel);
  }

  virtual ~SimpleClient() {
  }

  const std::string& serverEndpoint() const noexcept {
    return theServerEndpoint;
  }

 private:
  std::shared_ptr<grpc::Channel> theChannel;

 protected:
  std::unique_ptr<typename SERVER::Stub> theStub;
  const std::string                      theServerEndpoint;
};

} // namespace rpc
} // namespace uiiit
