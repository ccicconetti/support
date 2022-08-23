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

#include "gtest/gtest.h"

#include "testservice.grpc.pb.h"

#include "RpcSupport/simpleclient.h"
#include "RpcSupport/simpleserver.h"
#include "RpcSupport/utils.h"

#include <grpc++/grpc++.h>

#include <string>

namespace uiiit {
namespace rpc {

class SimpleServiceServer final : public SimpleServer
{
 private:
  class SimpleServerImpl final : public TestService::Service
  {
   public:
    SimpleServerImpl() = default;

   private:
    grpc::Status Get([[maybe_unused]] grpc::ServerContext* aContext,
                     [[maybe_unused]] const String*        aRequest,
                     [[maybe_unused]] String*              aResponse) override {
      aResponse->set_value(std::string("-") + aRequest->value() + "-");
      return grpc::Status::OK;
    }
  };

 public:
  explicit SimpleServiceServer(const std::string& aEndpoint)
      : SimpleServer(aEndpoint)
      , theServerImpl() {
    // noop
  }

 private:
  grpc::Service& service() override {
    return theServerImpl;
  }

 private:
  SimpleServerImpl theServerImpl;
};

class SimpleServiceClient final : public SimpleClient<TestService>
{
 public:
  explicit SimpleServiceClient(const std::string& aEndpoint)
      : SimpleClient<TestService>(aEndpoint) {
    // noop
  }

  std::string get(const std::string& aRequest) {
    grpc::ClientContext myContext;
    String              myRequest;
    String              myResponse;
    myRequest.set_value(aRequest);
    checkStatus(theStub->Get(&myContext, myRequest, &myResponse));
    return myResponse.value();
  }
};

struct TestRpc : public ::testing::Test {};

TEST_F(TestRpc, test_simple_client_server) {
  const std::string myEndpoint = "127.0.0.1:10000";

  SimpleServiceServer myServer(myEndpoint);
  myServer.run(false);

  auto myCorrect = false;
  for (auto i = 0; i < 100; i++) {
    SimpleServiceClient myClient(myEndpoint);
    const auto          myResponse = myClient.get("how do you do");
    if (myResponse == "-how do you do-") {
      myCorrect = true;
      break;
    }
  }
  if (not myCorrect) {
    FAIL() << "no response received from " << myEndpoint << " after 100 tries";
  }
}

} // namespace rpc
} // namespace uiiit
