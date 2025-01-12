#include "precompiled.hxx"
#include <algorithm>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

auto read_lists(const char* name)
{
    std::vector<int> left, right;

    std::ifstream input(name);
    unsigned l, r;
    input >> l >> r;
    while (input)
    {
        left.push_back(l);
        right.push_back(r);
        input >> l >> r;
    }
    return std::make_tuple(left, right);

}

unsigned sum_diffs(const char* name)
{
    auto [left, right] = read_lists(name);
    std::sort(begin(left), end(left));
    std::sort(begin(right), end(right));

    auto diff = [](int l, int r) {
        return std::abs(l - r);
    };

    auto diffs = std::views::zip_transform(diff, left, right);
    unsigned s = 0;
    for (auto d: diffs)
    {
        s += d;
    }

    return s;
}

unsigned sum_similarity(const char* name)
{
    auto [left, right] = read_lists(name);
    std::sort(begin(left), end(left));
    std::sort(begin(right), end(right));

    unsigned s = 0;
    for (auto n: left)
    {
        auto [lower, upper] = std::equal_range(begin(right), end(right), n);
        auto d = std::distance(lower, upper);
        s += n * d;
    }

    return s;
}



TEST_CASE("Sample")
{
    SUBCASE("Part 1")
    {
        CHECK(sum_diffs("sample.txt") == 11);

    }
    SUBCASE("Part 2")
    {
        CHECK(sum_similarity("sample.txt") == 31);
    }
}

TEST_CASE("Input")
{
    SUBCASE("Part 1")
    {
        CHECK(sum_diffs("input.txt") == 1651298);
    }
    SUBCASE("Part 2")
    {
        CHECK(sum_similarity("input.txt") == 21306195);
    }
}
