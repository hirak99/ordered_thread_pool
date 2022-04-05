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

// Tests that jobs are stable even if more than one thread pushes them.
TEST(ThreadPoolTest, MultiplePusher) {
  std::vector<int> visit_count(1000, 0);
  {
    ThreadPool thread_pool{10, 5};
    std::thread t1{[&] {
      for (int i = 0; i < 500; ++i) {
        thread_pool.Do([&visit_count, i] { ++visit_count[i]; });
      };
    }};
    std::thread t2{[&] {
      for (int i = 500; i < 1000; ++i) {
        thread_pool.Do([&visit_count, i] { ++visit_count[i]; });
      };
    }};
    t1.join();
    t2.join();
  }
  ASSERT_EQ(visit_count, std::vector<int>(1000, 1));
}

// Demonstrates passing parameters via unique_ptr.
TEST(ThreadPoolTest, UniquePtr) {
  std::vector<int> visit_count(50, 0);
  {
    ThreadPool thread_pool{10, 5};
    for (int i = 0; i < 50; ++i) {
      auto uptr = std::make_unique<int>(i);
      // Move to a shared ptr to make it copyable, as we send the function as a
      // copy.
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