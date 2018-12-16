//
// Created by Orange on 2018/12/6.
//

#include "buddy_allocator.h"
#include <sstream>

BuddyAllocator::BuddyAllocator(std::size_t min_block_size, std::size_t level)
    : min_block_size_(min_block_size),
      level_(level),
      capacity_(min_block_size * (1u << level)),
      base_(new char[capacity_]),
      blocks_((1u << (level + 1)) - 1)
{
}

BuddyAllocator::~BuddyAllocator()
{
    delete[] base_;
}

#ifndef RECURSIVE_VERSION

// iterative

char* BuddyAllocator::Allocate(std::size_t bytes)
{
    std::size_t block_index = 0;
    std::size_t block_size = capacity_;

    while (true)
    {
        if (blocks_[block_index] != Used)
        {
            if (block_size / 2 >= bytes && block_size / 2 >= min_block_size_) // go down
            {
                if (blocks_[block_index] == Unused)
                {
                    blocks_[block_index] = Split;
                }
                block_index = block_index * 2 + 1;
                block_size /= 2;
                continue;
            }
            else if (block_size >= bytes && blocks_[block_index] == Unused) // bingo!
            {
                MarkBlockAndParentAsUsed(block_index);
                return base_ + block_size * (block_index + 1 - capacity_ / block_size);
            }
        }

        while (block_index % 2 == 0) // go right (need go up first if not next sibling)
        {
            if (block_index == 0)
            {
                return nullptr; // root
            }
            block_index = (block_index - 1) / 2;
            block_size *= 2;
        }
        block_index += 1;
    }
}

bool BuddyAllocator::Free(char* ptr)
{
    if (ptr < base_ || ptr >= base_ + capacity_ || (ptr - base_) % min_block_size_ != 0)
    {
        return false;
    }

    std::size_t block_index = (1u << level_) + (ptr - base_) / min_block_size_ - 1; // bottom level

    while (blocks_[block_index] == Unused && block_index % 2 == 1)
    {
        block_index = (block_index - 1) / 2;
    }

    if (blocks_[block_index] == Used)
    {
        MarkBlockAndParentAsUnused(block_index);
        return true;
    }

    return false;
}

#else

// recursive

char* BuddyAllocator::Allocate(std::size_t bytes)
{
    return Allocate(bytes, 0, 0);
}

char* BuddyAllocator::Allocate(std::size_t bytes, std::size_t level, std::size_t block_index)
{
    switch (blocks_[block_index])
    {
        case Unused:
        {
            std::size_t block_size = capacity_ >> level;
            if (block_size / 2 >= bytes && level + 1 <= level_)
            {
                blocks_[block_index] = Split;
                // fall-through
            }
            else if (block_size >= bytes)
            {
                MarkBlockAndParentAsUsed(block_index);
                return base_ + block_size * (block_index + 1 - (1u << level));
            }
            else
            {
                return nullptr;
            }
        }

        case Split:
        {
            char* left = Allocate(bytes, level + 1, block_index * 2 + 1);
            return left ? left : Allocate(bytes, level + 1, block_index * 2 + 2);
        }

        case Used:
            return nullptr;
    }
}

bool BuddyAllocator::Free(char* ptr)
{
    if (ptr < base_ || ptr >= base_ + capacity_ || (ptr - base_) % min_block_size_ != 0)
    {
        return false;
    }

    std::size_t block_index = (1u << level_) + (ptr - base_) / min_block_size_ - 1; // bottom level
    return Free(block_index);
}

bool BuddyAllocator::Free(std::size_t block_index)
{
    switch (blocks_[block_index])
    {
        case Unused:
        {
            if (block_index % 2 == 1)
            {
                return Free((block_index - 1) / 2);
            }
        }

        case Split:
        {
            return false;
        }

        case Used:
            MarkBlockAndParentAsUnused(block_index);
            return true;
    }
}

#endif

void BuddyAllocator::MarkBlockAndParentAsUsed(std::size_t block_index)
{
    blocks_[block_index] = Used;

    while (block_index != 0)
    {
        if (blocks_[((block_index - 1) ^ 1) + 1] == Used)
        {
            block_index = (block_index - 1) / 2;
            blocks_[block_index] = Used;
        }
        else
        {
            break;
        }
    }
}

void BuddyAllocator::MarkBlockAndParentAsUnused(std::size_t block_index)
{
    blocks_[block_index] = Unused;

    while (block_index != 0)
    {
        if (blocks_[block_index] == Unused && blocks_[((block_index - 1) ^ 1) + 1] == Unused)
        {
            block_index = (block_index - 1) / 2;
            blocks_[block_index] = Unused;
        }
        else if (blocks_[((block_index - 1) ^ 1) + 1] == Used)
        {
            block_index = (block_index - 1) / 2;
            blocks_[block_index] = Split;
        }
        else
        {
            break;
        }
    }
}

std::string BuddyAllocator::Dump() const
{
    std::stringstream s;
    std::size_t level = 1;
    for (std::size_t i = 0; i < blocks_.size(); ++i)
    {
        switch (blocks_[i])
        {
            case Unused: s << 'o'; break;
            case Split:  s << '+'; break;
            case Used:   s << '.'; break;
        }
        if (i + 2 == (1u << level))
        {
            s << '\n';
            level += 1;
        }
    }
    s << '\n';
    return s.str();
}
