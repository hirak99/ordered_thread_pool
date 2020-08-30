#include "thread_pool.h"

#include <gtest/gtest.h>

#include <memory>

TEST(ThreadPoolTest, Threaded) {
  std::vector<int> visit_count(50, 0);
  {
    ThreadPool thread_pool{10, 5};
    for (int i = 0; i < 50; ++i) {
      thread_pool.Do([&visit_count, i] { ++visit_count[i]; });
    }
  }
  ASSERT_EQ(visit_count, std::vector<int>(50, 1));
}

// Demonstrates passing parameters via unique_ptr.
TEST(ThreadPoolTest, UniquePtr) {
  std::vector<int> visit_count(50, 0);
  {
    ThreadPool thread_pool{10, 5};
    for (int i = 0; i < 50; ++i) {
      auto uptr = std::make_unique<int>(i);
      // Move to a shared ptr to make it copyable, and hand over to the thread
      // pool.
      std::shared_ptr<int> sptr = std::move(uptr);
      thread_pool.Do([&visit_count, sptr] { ++visit_count[*sptr]; });
    }
  }
  ASSERT_EQ(visit_count, std::vector<int>(50, 1));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}