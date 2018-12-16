//
// Created by Orange on 2018/12/8.
//

#include "benchmark.h"

// Allocate
BASELINE_F(Allocate, Baseline, AllocateFixture, 100, 1)
{
    celero::DoNotOptimizeAway(p = nullptr);
}

BENCHMARK_F(Allocate, New, AllocateFixture, 100, 1)
{
    celero::DoNotOptimizeAway(p = new char[1024]);
}

BENCHMARK_F(Allocate, Malloc, AllocateFixture, 100, 1)
{
    celero::DoNotOptimizeAway(p = static_cast<char*>(std::malloc(1024)));
}

BENCHMARK_F(Allocate, Buddy, AllocateFixture, 100, 1)
{
    celero::DoNotOptimizeAway(p = allocator->Allocate(1024));
}

// Free
BASELINE_F(Free, Baseline, FreeFixture, 100, 1)
{
    for (std::size_t i = 0; i < iterations; ++i)
    {
        celero::DoNotOptimizeAway(p[0][i] = nullptr);
    }
}

BENCHMARK_F(Free, Delete, FreeFixture, 100, 1)
{
    for (std::size_t i = 0; i < iterations; ++i)
    {
        delete[] p[1][i];
    }
}

BENCHMARK_F(Free, Free, FreeFixture, 100, 1)
{
    for (std::size_t i = 0; i < iterations; ++i)
    {
        free(p[2][i]);
    }
}

BENCHMARK_F(Free, Buddy, FreeFixture, 100, 1)
{
    for (std::size_t i = 0; i < iterations; ++i)
    {
        allocator->Free(p[3][i]);
    }
}

CELERO_MAIN
