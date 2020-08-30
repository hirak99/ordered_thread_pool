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
  ThreadPool(int num_workers, int max_pending_jobs)
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
};

#endif  // THREAD_POOL_H
