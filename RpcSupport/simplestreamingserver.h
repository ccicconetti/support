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

#include "RpcSupport/simpleserver.h"
#include "Support/queue.h"

#include <glog/logging.h>

#include <grpc++/grpc++.h>

#include <algorithm>
#include <cassert>
#include <condition_variable>
#include <list>
#include <mutex>
#include <string>

namespace uiiit {
namespace rpc {

template <class MESSAGE>
class SimpleStreamingServer : public SimpleServer
{
  using Queue = support::Queue<std::shared_ptr<MESSAGE>>;

 protected:
  class SimpleStreamingServerImpl
  {
   public:
    explicit SimpleStreamingServerImpl(SimpleStreamingServer& aParent)
        : theParent(aParent) {
    }

    virtual ~SimpleStreamingServerImpl() = 0;

   protected:
    grpc::Status
    handle(grpc::ServerContext*                            aContext,
           const std::function<bool(const MESSAGE& aMsg)>& aWrite) {
      LOG(INFO) << "Connection from client: " << aContext->peer();

      Queue myQueue;
      theParent.subscribe(&myQueue);

      try {
        auto myOk = true;
        while (myOk) {
          myOk = aWrite(*myQueue.pop());
        }
      } catch (...) {
        // ignore
      }

      LOG(INFO) << "Client disconnected: " << aContext->peer();

      theParent.leave(&myQueue);
      return grpc::Status::OK;
    }

   private:
    SimpleStreamingServer& theParent;
  };

 public:
  explicit SimpleStreamingServer(const std::string& aServerEndpoint)
      : SimpleServer(aServerEndpoint)
      , theMutex()
      , theQueues() {
  }

  ~SimpleStreamingServer() override {
    std::unique_lock<std::mutex> myLock(theMutex);
    LOG_IF(WARNING, theQueues.size())
        << "closing " << theQueues.size() << " clients not disconnected";
    for (const auto& myQueue : theQueues) {
      myQueue->close();
    }
    theTerminatingCv.wait(myLock, [this]() { return theQueues.empty(); });
  }

 protected:
  void push(const std::shared_ptr<MESSAGE>& aMsg) {
    const std::lock_guard<std::mutex> myLock(theMutex);
    for (const auto& myQueue : theQueues) {
      assert(myQueue);
      myQueue->push(aMsg);
    }
  }

 private:
  void subscribe(Queue* aQueue) {
    assert(aQueue);

    const std::lock_guard<std::mutex> myLock(theMutex);
    assert(std::find_if(
               theQueues.begin(), theQueues.end(), [aQueue](Queue* aOther) {
                 return aOther == aQueue;
               }) == theQueues.end());
    theQueues.push_back(aQueue);
  }

  void leave(Queue* aQueue) {
    assert(aQueue);

    const std::lock_guard<std::mutex> myLock(theMutex);
    auto                              it =
        std::find_if(theQueues.begin(),
                     theQueues.end(),
                     [aQueue](Queue* aOther) { return aOther == aQueue; });
    assert(it != theQueues.end());
    (*it)->close();
    theQueues.erase(it);
    theTerminatingCv.notify_one();
  }

 private:
  mutable std::mutex      theMutex;
  std::list<Queue*>       theQueues;
  std::condition_variable theTerminatingCv;
};

template <class MESSAGE>
SimpleStreamingServer<
    MESSAGE>::SimpleStreamingServerImpl::~SimpleStreamingServerImpl() {
}

} // namespace rpc
} // end namespace uiiit
