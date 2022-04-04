// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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