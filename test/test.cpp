//
// Created by Orange on 2018/12/7.
//

#include "buddy_allocator.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#ifndef NDEBUG
  #include <iostream>
  #define PRINT_STATE std::cout << allocator.Dump() << std::endl;
#else
  #define PRINT_STATE
#endif

TEST_CASE("Test Allocate", "[allocate]")
{
    BuddyAllocator allocator(1024, 10); // 1 KiB * ( 2 ^ 10 ) = 1 MiB
    const char* base = allocator.Base();

    REQUIRE(allocator.Size() == 1024 * (1u << 10));

    SECTION("Test allocate small block")
    {
        for (int i = 0; i < (1u << 10); ++i)
        {
            REQUIRE(allocator.Allocate(1) == base + 1024 * i);
        }
        REQUIRE(allocator.Allocate(1) == nullptr);
    }

    SECTION("Test allocate bigger block")
    {
        for (int i = 0; i < (1u << 9); ++i)
        {
            REQUIRE(allocator.Allocate(1025) == base + 1024 * 2 * i);
        }
        REQUIRE(allocator.Allocate(1) == nullptr);
    }

    SECTION("Test allocate small and bigger block alternately")
    {
        for (int i = 0; i < (1u << 8); ++i)
        {
            REQUIRE(allocator.Allocate(1)    == base + 1024 * 4 * i);
            REQUIRE(allocator.Allocate(1025) == base + 1024 * 4 * i + 1024 * 2);
            REQUIRE(allocator.Allocate(1)    == base + 1024 * 4 * i + 1024);
            //PRINT_STATE
        }
        REQUIRE(allocator.Allocate(1) == nullptr);
    }

    SECTION("Test allocate half capacity")
    {
        REQUIRE(allocator.Allocate(1024 * (1u << 9)) == base);
        REQUIRE(allocator.Allocate(1) == base + allocator.Size() / 2);
    }

    SECTION("Test allocate whole capacity")
    {
        REQUIRE(allocator.Allocate(1024 * (1u << 10)) == base);
        REQUIRE(allocator.Allocate(1) == nullptr);
    }

    SECTION("Test allocate more than capacity")
    {
        REQUIRE(allocator.Allocate(1024 * (1u << 10) + 1) == nullptr);
    }
}

TEST_CASE("Test Free", "[free]")
{
    BuddyAllocator allocator(1024, 10); // 1 KiB * ( 2 ^ 10 ) = 1 MiB
    const char* base = allocator.Base();

    REQUIRE(allocator.Size() == 1024 * (1u << 10));

    SECTION("Test allocate and free immediately")
    {
        for (int i = 0; i < (1u << 11); ++i)
        {
            char* p = allocator.Allocate(1);
            REQUIRE(p == base);
            REQUIRE(allocator.Free(p));
        }
        REQUIRE(allocator.Allocate(1024 * (1u << 10)) == base);
    }

    SECTION("Test allocate all and then free")
    {
        char* p[1u << 10];
        for (int i = 0; i < (1u << 10); ++i)
        {
            p[i] = allocator.Allocate(1);
            REQUIRE(p[i] == base + 1024 * i);
        }
        REQUIRE(allocator.Allocate(1) == nullptr);

        for (int i = 0; i < (1u << 10); ++i)
        {
            REQUIRE(allocator.Free(p[i]));
            //PRINT_STATE
        }
        REQUIRE(allocator.Allocate(1024 * (1u << 10)) == base);
    }

    SECTION("Test allocate whole capacity and free")
    {
        char* p = allocator.Allocate(1024 * (1u << 10));
        REQUIRE(p == base);
        REQUIRE(allocator.Allocate(1) == nullptr);

        REQUIRE(allocator.Free(p));
        REQUIRE(allocator.Allocate(1024 * (1u << 10)) == base);
    }

    SECTION("Test free and re allocate")
    {
        char* p[1u << 10];
        for (int i = 0; i < (1u << 9); ++i)
        {
            p[i] = allocator.Allocate(1025);
            REQUIRE(p[i] == base + 1024 * 2 * i);
        }
        REQUIRE(allocator.Allocate(1) == nullptr);

        for (int i = 0; i < (1u << 9); ++i)
        {
            REQUIRE(allocator.Free(p[i]));
            REQUIRE(allocator.Allocate(1025) == p[i]);
        }
    }

    SECTION("Test free and re allocate more complicatedly")
    {
        char* p[(1u << 8) * 3];
        for (int i = 0; i < (1u << 8); ++i)
        {
            REQUIRE((p[3 * i]     = allocator.Allocate(1))    == base + 1024 * 4 * i);
            REQUIRE((p[3 * i + 1] = allocator.Allocate(1025)) == base + 1024 * 4 * i + 1024 * 2);
            REQUIRE((p[3 * i + 2] = allocator.Allocate(1))    == base + 1024 * 4 * i + 1024);
        }
        REQUIRE(allocator.Allocate(1) == nullptr);

        for (int i = 0; i < (1u << 8); ++i)
        {
            REQUIRE(allocator.Free(p[3 * i]));
            REQUIRE(allocator.Free(p[3 * i + 2]));
            REQUIRE(allocator.Allocate(1025) == base + 1024 * 4 * i);
        }
        REQUIRE(allocator.Allocate(1) == nullptr);

        for (int i = 0; i < (1u << 8); ++i)
        {
            REQUIRE(allocator.Free(p[3 * i + 1]));
            REQUIRE(allocator.Allocate(1) == base + 1024 * 4 * i + 1024 * 2);
            REQUIRE(allocator.Allocate(1) == base + 1024 * 4 * i + 1024 * 3);
        }
        REQUIRE(allocator.Allocate(1) == nullptr);
    }
}
