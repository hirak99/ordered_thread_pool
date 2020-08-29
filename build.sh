set -uexo pipefail
mkdir -p build
cd build
# g++ -std=c++17 -g ../demo.cpp -lpthread -o ./demo
# ./demo

cmake -DCMAKE_BUILD_TYPE=Debug .. -GNinja
ninja
GTEST_COLOR=1 ctest -V
