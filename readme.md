# OrderedThreadPool

This offers a threadpool implementation, with synchronization.

## What this does
Imagine you want to parallelize the following code -

```c++
while (...) {
  std::cout << CostlyFn(input)) << std::endl;
}
```

With the constraint that outputs must be generated in order.

With this you can write it out like this -

```c++
OrderedThredPool<std::string> pool{10, 1};
while (...) {
  pool.Do(
    [&input] { return CostlyFn(input); },
    [](const std::string& out) {
      std::cout << out << std::endl;
    });
}
```
This maintains order. I.e. if `CostlyFn(1)` is called before `CostlyFn(2)`, then the results are also printed in the same order.

## Features

* Avoids thread spawn overhead by instantiating and maintaining threads on construction.
* Allows disabiling all threading by setting number of threads to 0. This allows rapid prototyping, and can help in debugging threaded logic in a client.
* Accepts a limit to throttle main thread if queue gets filled up faster than the jobs are finished.

# Dependencies

## GoogleTest

This uses GoogleTest for unit testing. It may be downloaded with the following in Debian distros.

```bash
sudo apt install libgtest-dev build-essential cmake
mkdir gtest_build && cd gtest_build
cmake /usr/src/googletest
sudo cmake --build . --target install
cd .. && rm -r gtest_build
```
