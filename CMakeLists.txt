cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(demo demo.cpp)
target_link_libraries(demo pthread)

# add_executable(ordered_thread_pool_test ordered_thread_pool_test.cpp)
# target_link_libraries(ordered_thread_pool_test ${GTEST_LIBRARIES} pthread)
