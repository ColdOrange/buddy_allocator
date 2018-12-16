add_executable(benchmark benchmark.cpp)

target_include_directories(benchmark
        PRIVATE ${CMAKE_SOURCE_DIR}/buddy
        PRIVATE ${CMAKE_SOURCE_DIR}/third_party/Celero/include)

target_link_libraries(benchmark
        PRIVATE buddy_allocator
        PRIVATE celero)

target_compile_options(benchmark PRIVATE -O3)
