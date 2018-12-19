#include "Edge/composer.h"
#include "Edge/computer.h"
#include "Edge/edgeclient.h"
#include "Edge/edgecomputer.h"
#include "Edge/edgecomputerclient.h"
#include "Edge/edgecomputerserver.h"
#include "Edge/edgecontrollerclient.h"
#include "Edge/edgecontrollerserver.h"
#include "Edge/edgedispatcher.h"
#include "Edge/edgerouter.h"
#include "Edge/forwardingtableserver.h"
#include "Edge/ptimeestimatorfactory.h"
#include "Support/chrono.h"
#include "Support/conf.h"

#include "testutils.h"

#include "gtest/gtest.h"

#include <deque>
#include <memory>
#include <string>

namespace uiiit {
namespace edge {

struct TestLambdaTransaction : public ::testing::Test {

  struct System {
    enum Type { ROUTER = 0, DISPATCHER = 1 };

    System(const Type aType, const std::string& aSubtype)
        : theControllerEndpoint("127.0.0.1:6475")
        , theRouterEndpoint("127.0.0.1:6473")
        , theForwardingEndpoint("127.0.0.1:6474")
        , theComputerEndpoint("127.0.0.1:10000")
        , theUtilEndpoint("127.0.0.1:6476")
        , theNumThreads(5)
        , theController(theControllerEndpoint)
        , theUtilServer(theUtilEndpoint)
        , theComputer(theComputerEndpoint,
                      theNumThreads,
                      [this](const std::map<std::string, double>& aUtil) {
                        theUtilServer.add(aUtil);
                      })
        , theRouter(aType == ROUTER ?
                        new EdgeRouter(
                            theRouterEndpoint,
                            theForwardingEndpoint,
                            theControllerEndpoint,
                            theNumThreads,
                            support::Conf("max-pending-clients=2"),
                            support::Conf("type=random"),
                            support::Conf("type=trivial,period=10,stat=mean")) :
                        nullptr)
        , theDispatcher(
              aType == DISPATCHER ?
                  new EdgeDispatcher(
                      theRouterEndpoint,
                      theForwardingEndpoint,
                      theControllerEndpoint,
                      theNumThreads,
                      support::Conf("max-pending-clients=2"),
                      support::Conf(
                          PtimeEstimatorFactory::defaultConf(aSubtype))) :
                  nullptr)
        , theForwardingTableServer(theForwardingEndpoint,
                                   aType == ROUTER ? theRouter->table() :
                                                     theDispatcher->table())
        , theUtils() {
      // configure the computer
      Composer()(
          support::Conf("type=intel-server,num-containers=1,num-workers=1"),
          theComputer.computer());

      // start all the servers (in a non-blocking fashion, obviously)
      theController.run(false);
      theUtilServer.run(false);
      theComputer.run();
      if (theRouter) {
        theRouter->run();
      }
      if (theDispatcher) {
        theDispatcher->run();
      }
      theForwardingTableServer.run(false);

      // announce the computer to the controller
      EdgeControllerClient myControllerClient(theControllerEndpoint);
      myControllerClient.announceComputer(
          theComputerEndpoint, *theComputer.computer().containerList());

      // save utilization values in memory
      theUtilClient.reset(new EdgeComputerClient(theUtilEndpoint));
      theUtilThread = std::thread([this]() {
        try {
          theUtilClient->StreamUtil(
              [this](const std::string& aProcessor, const float aUtil) {
                assert(aProcessor == "xeon");
                theUtils.push_back(aUtil);
              });
        } catch (const std::exception& aErr) {
          LOG(WARNING) << "caught exception: " << aErr.what();
        }
      });

      LOG(INFO) << "starting test";
    }

    ~System() {
      LOG(INFO) << "terminating test";

      theUtilClient->cancel();

      assert(theUtilThread.joinable());
      theUtilThread.join();
    }

    const std::string theControllerEndpoint;
    const std::string theRouterEndpoint;
    const std::string theForwardingEndpoint;
    const std::string theComputerEndpoint;
    const std::string theUtilEndpoint;
    const size_t      theNumThreads;

    EdgeControllerServer                theController;
    EdgeComputerServer                  theUtilServer;
    EdgeComputer                        theComputer;
    std::unique_ptr<EdgeRouter>         theRouter;
    std::unique_ptr<EdgeDispatcher>     theDispatcher;
    ForwardingTableServer               theForwardingTableServer;
    std::deque<float>                   theUtils;
    std::thread                         theUtilThread;
    std::unique_ptr<EdgeComputerClient> theUtilClient;
  };
};

TEST_F(TestLambdaTransaction, test_endtoend) {
  const size_t N = 50;

  const std::list<System::Type> myTypes({System::ROUTER, System::DISPATCHER});

  const std::map<System::Type, std::set<std::string>> mySubtypes({
      {System::ROUTER, {""}},
      {System::DISPATCHER, PtimeEstimatorFactory::types()},
  });

  for (const auto& myType : myTypes) {
    const auto it = mySubtypes.find(myType);
    assert(it != mySubtypes.end());
    for (const auto& mySubtype : it->second) {
      LOG(INFO) << "*** Running end-to-end experiment with an "
                << (myType == System::ROUTER ? "edge router" :
                                               "edge dispatcher")
                << " " << mySubtype;

      System mySystem(myType, mySubtype);

      EdgeClient    myClient(mySystem.theRouterEndpoint);
      LambdaRequest myReq("clambda0", std::string(1000, 'A'));

      support::Chrono myChrono(false);
      for (size_t i = 0; i < N; i++) {
        myChrono.start();
        const auto myResp = myClient.RunLambda(myReq, false);
        ASSERT_EQ(mySystem.theComputerEndpoint, myResp.theResponder);
        ASSERT_EQ("OK", myResp.theRetCode);
        ASSERT_EQ(std::string(1000, 'A'), myResp.theOutput);
        ASSERT_EQ(0u, myResp.theDataOut.size());
        ASSERT_NEAR(myResp.theProcessingTime, myChrono.stop() * 1000, 10);
      }

      for (const auto& myUtil : mySystem.theUtils) {
        ASSERT_NEAR(0.05, myUtil, 0.01);
      }
    }
  }
}

} // namespace edge
} // namespace uiiit
