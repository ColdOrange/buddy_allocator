//
// Created by Orange on 2018/12/6.
//

#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <cstddef>
#include <vector>
#include <string>

// define this to use recursive version (a little bit slower)
//#define RECURSIVE_VERSION

class BuddyAllocator
{
public:
    BuddyAllocator(std::size_t min_block_size, std::size_t level);
    ~BuddyAllocator();

    BuddyAllocator(const BuddyAllocator&) = delete;
    BuddyAllocator& operator=(const BuddyAllocator&) = delete;

    char* Allocate(std::size_t bytes);
    bool Free(char* ptr);

    const char* Base() const { return base_; }
    std::size_t Size() const { return capacity_; }

    std::string Dump() const;

private:
    enum BlockStatus : uint8_t
    {
        Unused = 0,
        Split,
        Used
    };

    void MarkBlockAndParentAsUsed(std::size_t block_index);
    void MarkBlockAndParentAsUnused(std::size_t block_index);

#ifdef RECURSIVE_VERSION
    char* Allocate(std::size_t bytes, std::size_t level, std::size_t block_index);
    bool Free(std::size_t block_index);
#endif

    std::size_t min_block_size_;
    std::size_t level_;
    std::size_t capacity_;
    char* base_;
    std::vector<BlockStatus> blocks_;
};

#endif // BUDDY_ALLOCATOR_H
