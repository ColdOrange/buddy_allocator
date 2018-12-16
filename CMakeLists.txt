cmake_minimum_required(VERSION 3.12)

project(buddy_allocator)

option(BUDDY_ENABLE_TEST "enable testing for buddy allocator" ON)
option(BUDDY_ENABLE_BENCHMARK "enable benchmarking for buddy allocator" ON)

add_subdirectory(buddy)

if(BUDDY_ENABLE_TEST)
    add_subdirectory(test)
endif()

if(BUDDY_ENABLE_BENCHMARK)
    add_subdirectory(third_party/Celero)
    add_subdirectory(benchmark)
endif()
