set -uexo pipefail
mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Debug .. -GNinja
ninja

GTEST_COLOR=1 ctest -V
