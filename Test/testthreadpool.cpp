#include "Support/threadpool.h"

#include "gtest/gtest.h"

#include <atomic>

namespace uiiit {
namespace support {

struct TestThreadPool : public ::testing::Test {
  struct Callable {
    Callable(std::atomic<int>& aCounter)
        : theCounter(aCounter) {
    }

    void operator()() {
      theCounter++;
    }

    std::atomic<int>& theCounter;
  };

  struct Stoppable : public Callable {
    Stoppable(std::atomic<int>& aCounter, bool& aStopped)
        : Callable(aCounter)
        , theStopped(aStopped) {
    }

    void stop() {
      theStopped = true;
    }

    bool theStopped;
  };

  struct CallableThrowing {
    void operator()() {
      throw std::runtime_error("Oh no!");
    }
  };

  TestThreadPool()
      : theCounter{0} {
  }

  std::atomic<int> theCounter;
};

TEST_F(TestThreadPool, test_empty_not_started) {
  ThreadPool<Callable> myPool;
  ASSERT_TRUE(myPool.wait().empty());
}

TEST_F(TestThreadPool, test_not_empty_not_started) {
  ThreadPool<Callable> myPool;
  myPool.add(Callable(theCounter));
  ASSERT_TRUE(myPool.wait().empty());
  ASSERT_EQ(0, theCounter.load());
}

TEST_F(TestThreadPool, test_not_empty_started) {
  ThreadPool<Callable> myPool;
  for (auto i = 0; i < 5; i++) {
    myPool.add(Callable(theCounter));
  }
  myPool.start();
  ASSERT_TRUE(myPool.wait().empty());
  ASSERT_EQ(5, theCounter.load());
}

TEST_F(TestThreadPool, test_empty_started) {
  ThreadPool<Callable> myPool;
  myPool.start();
  ASSERT_TRUE(myPool.wait().empty());
  ASSERT_EQ(0, theCounter.load());
}

TEST_F(TestThreadPool, test_callable_throwing) {
  ThreadPool<CallableThrowing> myPool;
  std::list<std::string>       myExpected;
  for (auto i = 0; i < 5; i++) {
    myPool.add(CallableThrowing());
    myExpected.push_back("Oh no!");
  }
  myPool.start();
  ASSERT_EQ(myExpected, myPool.wait());
}

TEST_F(TestThreadPool, test_cannot_start_twice) {
  ThreadPool<Callable> myPool;
  ASSERT_NO_THROW(myPool.start());
  ASSERT_THROW(myPool.start(), std::runtime_error);
  ASSERT_NO_THROW(myPool.wait());
  ASSERT_NO_THROW(myPool.wait());
}

TEST_F(TestThreadPool, test_cannot_start_after_wait) {
  ThreadPool<Callable> myPool;
  ASSERT_NO_THROW(myPool.wait());
  ASSERT_THROW(myPool.start(), std::runtime_error);
}

TEST_F(TestThreadPool, test_instantiate_with_pointer) {
  ThreadPool<Callable*> myPool;
  auto                  myCallable = std::make_unique<Callable>(theCounter);
  myPool.add(myCallable.get());
  myPool.start();
  ASSERT_TRUE(myPool.wait().empty());
  ASSERT_EQ(1, theCounter.load());
}

TEST_F(TestThreadPool, test_instantiate_with_unique_ptr) {
  ThreadPool<std::unique_ptr<Callable>> myPool;
  myPool.add(std::make_unique<Callable>(theCounter));
  myPool.start();
  ASSERT_TRUE(myPool.wait().empty());
  ASSERT_EQ(1, theCounter.load());
}

TEST_F(TestThreadPool, test_wait_before_stop) {
  ThreadPool<Stoppable> myPool;
  bool                  myStopped;
  myPool.add(Stoppable(theCounter, myStopped));
  myPool.start();
  ASSERT_NO_THROW(myPool.wait());
  ASSERT_FALSE(myStopped);
}

TEST_F(TestThreadPool, test_stop_before_wait) {
  ThreadPool<Stoppable> myPool;
  bool                  myStopped;
  myPool.add(Stoppable(theCounter, myStopped));
  myPool.start();
  ASSERT_NO_THROW(myPool.stop());
  ASSERT_NO_THROW(myPool.wait());
  ASSERT_FALSE(myStopped);
}

TEST_F(TestThreadPool, test_cannot_stop_before_start) {
  ThreadPool<Stoppable> myPool;
  ASSERT_THROW(myPool.stop(), std::runtime_error);
}

} // namespace support
} // namespace uiiit
