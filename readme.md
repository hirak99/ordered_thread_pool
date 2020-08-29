# Dependencies

## GoogleTest

```
sudo apt install libgtest-dev build-essential cmake
mkdir gtest_build && cd gtest_build
cmake /usr/src/googletest
sudo cmake --build . --target install
cd .. && rm -r gtest_build
```
