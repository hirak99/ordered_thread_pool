#include <chrono>
#include <iostream>
#include <thread>

#include "../ordered_thread_pool.h"

// Uses threadpool on simulated threads.
int main() {
  OrderedThreadPool<int> pool{10, 5};
  std::cout << "Start..." << std::endl;
  for (int i = 0; i < 50; ++i) {
    pool.Do(
        [i]() -> int {
          std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 200));
          return i;
        },
        [i](int k) { std::cout << "Result: " << k << std::endl; });
  }
  std::cout << "Fin." << std::endl;
  return 0;
  // The pending jobs will continue to be processed. Destructor of thread_pool
  // will block here till all the tasks are finished.
}
