#include "Edge/edgemessages.h"
#include "Edge/lambda.h"
#include "Edge/processor.h"

#include "gtest/gtest.h"

namespace uiiit {
namespace edge {

struct TestLambda : public ::testing::Test {
  TestLambda()
      : theName("lambda1") {
  }
  const std::string theName;
};

TEST_F(TestLambda, test_ctor) {
  ASSERT_NO_THROW(Lambda(theName, [](const Processor&, const LambdaRequest) {
    return LambdaRequirements{0, 0};
  }));
}

TEST_F(TestLambda, test_proportional) {
  Lambda myLambda(theName, ProportionalRequirements(1, -100, 2, -200));

  Processor myProc("cpu1", ProcessorType::GenericCpu, 1000, 1, 42);

  {
    LambdaRequest myRequest(theName, std::string(1000, 'x'));
    const auto    myReq = myLambda.requirements(myProc, myRequest);
    ASSERT_EQ(900u, myReq.theOperations);
    ASSERT_EQ(1800u, myReq.theMemory);
  }
  {
    LambdaRequest myRequest(theName, std::string(100, 'x'));
    const auto    myReq = myLambda.requirements(myProc, myRequest);
    ASSERT_EQ(0u, myReq.theOperations);
    ASSERT_EQ(0u, myReq.theMemory);
  }
}

TEST_F(TestLambda, test_fixed) {
  Lambda myLambda(theName, FixedRequirements(1, 2));

  Processor myProc("cpu1", ProcessorType::GenericCpu, 1000, 1, 42);

  LambdaRequest myRequest(theName, std::string(1000, 'x'));
  const auto    myReq = myLambda.requirements(myProc, myRequest);
  ASSERT_EQ(1u, myReq.theOperations);
  ASSERT_EQ(2u, myReq.theMemory);
}

} // namespace edge
} // namespace uiiit
