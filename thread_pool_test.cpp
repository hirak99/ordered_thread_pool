#include "thread_pool.h"

#include <gtest/gtest.h>

TEST(ThreadPoolTest, Threaded) {
  std::vector<int> visit_count(50, 0);
  {
    ThreadPool thread_pool{10, 5};
    for (int i = 0; i < 50; ++i) {
      thread_pool.Do([&visit_count, i] { ++visit_count[i]; });
    }
  }
  // ASSERT_TRUE(std::all_of(visit_count.begin(), visit_count.end(),
  //                         [](int value) { return value == 1; }));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}