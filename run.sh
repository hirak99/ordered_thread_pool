set -uexo pipefail
mkdir -p build
cd build
# g++ -std=c++17 -g ../demo.cpp -lpthread -o ./demo
# ./demo

cmake ..
make
./demo

