#include "ordered_thread_pool.h"

#include <gtest/gtest.h>

// Runs through a given a thread pool. The maximum value seen so far is held at
// *max. Does not wait for the thread pool to finish.
void RunTest1(int num_entries, OrderedThreadPool<int>* pool, int* max) {
  for (int i = 0; i < num_entries; ++i) {
    pool->Do([i] { return i; },
             [i, max](const int& k) {
               // Assert the outputs come in correct order.
               ASSERT_EQ(k, i);
               // Update the max output seen so far.
               if (k > *max) {
                 *max = k;
               }
             });
  }
}

TEST(OrderedThreadPoolTest, Unthreaded) {
  int max = 0;
  OrderedThreadPool<int> thread_pool{0};
  RunTest1(50, &thread_pool, &max);
  ASSERT_EQ(max, 49);
}

TEST(OrderedThreadPoolTest, Threaded) {
  int max = 0;
  {
    OrderedThreadPool<int> thread_pool{10, 0};
    RunTest1(50, &thread_pool, &max);
  }
  ASSERT_EQ(max, 49);
}

TEST(OrderedThreadPoolTest, ThreadedWithQueueLimit) {
  int max = 0;
  {
    OrderedThreadPool<int> thread_pool{10, 5};
    RunTest1(50, &thread_pool, &max);
    // Minimum possible value of the max so far must be 49 - 10 - 5, with 10
    // threads running, 5 pending.
    ASSERT_GE(max, 34);
  }
  ASSERT_EQ(max, 49);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}