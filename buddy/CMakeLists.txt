add_library(buddy_allocator buddy_allocator.cpp)

target_compile_features(buddy_allocator PUBLIC cxx_deleted_functions cxx_strong_enums)

target_compile_options(buddy_allocator PRIVATE -Wall -Werror -Wextra -O3)
