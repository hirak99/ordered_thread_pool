# OrderedThreadPool

This is a lightweight performant implementation of threadpool with synchronization.

An use case is when you want to queue multiple jobs to run in parallel, but their outputs displayed in the same order as they were queued.

The implementtion is provided as a header-only library.

## Quick Example
Imagine you want to parallelize the following code -

```c++
while (...) {
  std::cout << CostlyFn(input)) << std::endl;
}
```

Also imagine that you want outputs to be printed in order.

With OrderedThreadPool, you can achieve it like below -

```c++
OrderedThredPool<std::string> pool{10, 1};
while (...) {
  pool.Do(
    [input] { return CostlyFn(input); },
    [](const std::string& out) {
      std::cout << out << std::endl;
    });
}
```

Note how this reads very similar to the original code. This does a few things -
* Ensures that the output is displayed in same order as `Do()` was called.
* Spawns 10 threads and distributes incoming tasks.
* If `CostlyFn(...)` takes too long and all threads are busy, this blocks further input. This behavior is governed by the second construction parameter, and can be turned off by passing 0.

## Features

1. Avoids thread spawn overhead by instantiating and maintaining threads on construction.
2. Allows disabiling all threading by setting number of threads to 0. This allows rapid prototyping, and can help in debugging threaded logic in a client.
3. Accepts a limit to throttle main thread if queue gets filled up faster than the jobs are finished.
4. It does not require the full list of jobs to be known as they are being queued.

## Detailed Specification

Imagins a open ended list of jobs that come in pairs (A1, B1), (A2, B2), (A3, B3), ...

We need to run these jobs with the constraints -
* All Ai's can be parallelized.
* All Bi's must be run in order.
* Each Bi must use output of Ai.

This system enables such an operation.

Ai's indicate a costly job, Bi's indicate a fast job - e.g. writing to some stream.

# Dependencies

The libraries are header-only, and none of the following dependencies are required to use them in your project.

Nonetheless, they are required to build the tests.

## Ninja

Ninja is a fast build system alternative to make.

Install with -
```bash
sudo apt install ninja-build
```

## GoogleTest

This uses GoogleTest for unit testing. Following commands install it in Debian distros -

```bash
sudo apt install libgtest-dev build-essential cmake
mkdir gtest_build && cd gtest_build
cmake /usr/src/googletest
sudo cmake --build . --target install
cd .. && rm -r gtest_build
```
