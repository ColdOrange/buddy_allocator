add_executable(test test.cpp)
target_include_directories(test
        PRIVATE ${CMAKE_SOURCE_DIR}/buddy
        PRIVATE ${CMAKE_SOURCE_DIR}/third_party/Catch/single_include)
target_link_libraries(test PRIVATE buddy_allocator)
