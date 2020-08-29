#include <chrono>
#include <iostream>
#include <thread>

#include "ordered_thread_pool.h"

int main() {
  OrderedThreadPool<int> thread_pool{5};
  std::cout << "Start..." << std::endl;
  for (int i = 0; i < 50; ++i) {
    thread_pool.Do(
        [i]() -> int {
          std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 500));
          return i;
        },
        [](int k) { std::cout << "Result: " << k << std::endl; });
  }
  // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  std::cout << "Fin." << std::endl;
  return 0;
}