cmake_minimum_required(VERSION 3.10)

project(OrderedThreadPool HOMEPAGE_URL https://github.com/hirak99/ordered_thread_pool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

# Force color on compiler... this is disabled when compiled by Ninja.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Demos.
add_executable(random_delay_ordered src/demos/random_delay_ordered.cpp)
target_link_libraries(random_delay_ordered PRIVATE pthread)
add_executable(random_delay src/demos/random_delay.cpp)
target_link_libraries(random_delay PRIVATE pthread)

enable_testing()
add_executable(ordered_thread_pool_test src/ordered_thread_pool_test.cpp)
target_link_libraries(ordered_thread_pool_test PRIVATE ${GTEST_LIBRARIES} pthread)
gtest_add_tests(TARGET ordered_thread_pool_test)
add_executable(thread_pool_test src/thread_pool_test.cpp)
target_link_libraries(thread_pool_test PRIVATE ${GTEST_LIBRARIES} pthread)
gtest_add_tests(TARGET thread_pool_test)
