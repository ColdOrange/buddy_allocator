//
// Created by Orange on 2018/12/10.
//

#ifndef BUDDY_ALLOCATOR_BENCHMARK_H
#define BUDDY_ALLOCATOR_BENCHMARK_H

#include <cmath>
#include <cstdlib>
#include "buddy_allocator.h"
#include "celero/Celero.h"

class AllocateFixture : public celero::TestFixture
{
public:
    AllocateFixture() = default;

    std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override
    {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace {
            {1 << 10, 1 << 10},
            {1 << 15, 1 << 15},
        };
        return problemSpace;
    }

    void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
    {
        allocator = new BuddyAllocator(1024, static_cast<std::size_t>(std::log2(experimentValue.Value)));
    }

    BuddyAllocator* allocator {nullptr};
    char* p {nullptr};
};

class FreeFixture : public celero::TestFixture
{
public:
    FreeFixture() = default;

    std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override
    {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace {1 << 10, 1 << 15};
        return problemSpace;
    }

    void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
    {
        iterations = static_cast<size_t>(experimentValue.Value);
        allocator = new BuddyAllocator(1024, static_cast<std::size_t>(std::log2(iterations)));
        for (auto& x : p)
        {
            x.reserve(iterations);
        }
        for (std::size_t i = 0; i < iterations; ++i)
        {
            p[1].emplace_back(new char[1024]);
            p[2].emplace_back(static_cast<char*>(std::malloc(1024)));
            p[3].emplace_back(allocator->Allocate(1024));
        }
    }

    BuddyAllocator* allocator {nullptr};
    std::size_t iterations {0};
    std::vector<char*> p[4];
};

#endif // BUDDY_ALLOCATOR_BENCHMARK_H
