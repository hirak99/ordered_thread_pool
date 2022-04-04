/*
 * Copyright 2022 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Uses OrderedThreadPool to define a simple thread pool.
//
// Ordered execution is not guaranteed.
//
// Example -
//
//   pool = ThreadPool{10, 1};
//   while (...) {
//     pool.Do(CostlyFn);
//   }
//
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "ordered_thread_pool.h"

class ThreadPool : public OrderedThreadPool<int> {
 public:
  ThreadPool(int num_workers, int max_pending_jobs = 1)
      : OrderedThreadPool(num_workers, max_pending_jobs) {}
  void Do(std::function<void()> fn) {
    OrderedThreadPool::Do(
        // It is important to pass the fn _not_ by reference, since it will be
        // executed with a delay.
        [fn] {
          fn();
          return 0;
        },
        [](int) {});
  }

 private:
  // Hide the inherited form of Do that takes two arguments.
  using OrderedThreadPool::Do;
};

#endif  // THREAD_POOL_H
