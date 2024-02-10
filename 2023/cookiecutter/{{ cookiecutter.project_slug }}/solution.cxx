#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"


TEST_CASE("Sample")
{
    SUBCASE("Part 1")
    {
        CHECK(6 * 7 == 42);

    }
    SUBCASE("Part 2")
    {
        CHECK(6 + 7 == 13);
    }
}

TEST_CASE("Input")
{
    SUBCASE("Part 1")
    {
        CHECK(6 * 7 == 24);
    }
    SUBCASE("Part 2")
    {
        CHECK(6 + 7 == 24);
    }
}
