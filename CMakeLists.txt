cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Demos.
add_executable(random_delay_ordered demos/random_delay_ordered.cpp)
target_link_libraries(random_delay_ordered PRIVATE pthread)
add_executable(random_delay demos/random_delay.cpp)
target_link_libraries(random_delay PRIVATE pthread)

enable_testing()
add_executable(ordered_thread_pool_test ordered_thread_pool_test.cpp)
target_link_libraries(ordered_thread_pool_test PRIVATE ${GTEST_LIBRARIES} pthread)
gtest_add_tests(TARGET ordered_thread_pool_test)
add_executable(thread_pool_test thread_pool_test.cpp)
target_link_libraries(thread_pool_test PRIVATE ${GTEST_LIBRARIES} pthread)
gtest_add_tests(TARGET thread_pool_test)
