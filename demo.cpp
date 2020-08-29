#include <chrono>
#include <iostream>
#include <thread>

#include "ordered_thread_pool.h"

// Uses threadpool on simulated threads.
int main() {
  OrderedThreadPool<int> thread_pool{10, 5};
  std::cout << "Start..." << std::endl;
  // Used to ensure that the next output comes in correct order.
  int next_expected_val = 0;
  for (int i = 0; i < 50; ++i) {
    thread_pool.Do(
        [i]() -> int {
          std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 200));
          return i;
        },
        [i, &next_expected_val](int k) {
          std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
          std::cout << "Result: " << k << std::endl;
          if (next_expected_val != k) {
            std::cout << "ERROR: Incorrect order detected" << std::endl;
            exit(-1);
          }
          ++next_expected_val;
        });
  }
  // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  std::cout << "Fin." << std::endl;
  return 0;
}
