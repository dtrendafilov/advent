#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

using Stones = std::list<int64_t>;

int64_t to_number(const std::string_view input)
{
    int64_t number;
    auto result = std::from_chars(input.data(), input.data() + input.size(), number);
    CHECK(result.ec == std::errc{});
    return number;
}


auto read_stones(const std::string_view input)
{
    Stones result;
    size_t start = 0;
    size_t end = input.find(' ');
    
    while (end != std::string_view::npos) {
        result.emplace_back(to_number(input.substr(start, end - start)));
        start = end + 1;
        end = input.find(' ', start);
    }
    
    // Add the last part if it exists
    if (start < input.length()) {
        result.emplace_back(to_number(input.substr(start)));
    }
    
    return result;
}

int64_t split10(int64_t number)
{
    int result = 0;
    while (number > 0)
    {
        number /= 10;
        ++result;
    }
    return result % 2 == 0? int64_t(powf(10, result / 2)) : 0;
}

void update_stones(Stones& stones)
{
    auto current = begin(stones);
    while (current != end(stones))
    {
        auto next = std::next(current);
        if (*current == 0)
        {
            *current = 1;
        }
        else if (auto split = split10(*current))
        {
            auto number = *current;
            *current = number / split;
            stones.insert(next, number % split);
        }
        else 
        {
            *current *= 2024;
        }
        current = next;
    }
}

void simulate(Stones& stones, int runs)
{
    for (int i = 0; i < runs; ++i)
    {
        update_stones(stones);
    }
}

using Cache = std::unordered_map<int64_t, std::unordered_map<int, size_t>>;

Cache g_cache;

size_t simulate_stone(int64_t stone, int runs)
{
    if (runs == 0)
    {
        return 1;
    }

    if (auto it = g_cache.find(stone); it != end(g_cache) && it->second.find(runs) != end(it->second))
    {
        return it->second[runs];
    }

    if (stone == 0)
    {
        auto r = simulate_stone(1, runs - 1);
        return g_cache[stone][runs] = r;
    }
    else if (auto split = split10(stone))
    {
        auto r = simulate_stone(stone / split, runs - 1) + simulate_stone(stone % split, runs - 1);
        return g_cache[stone][runs] = r;
    }
    else
    {
        auto r = simulate_stone(stone * 2024, runs - 1);
        return g_cache[stone][runs] = r;
    }
}

size_t simulate_2(const Stones& stones, int runs)
{
    size_t result = 0;
    for (auto stone : stones)
    {
        result += simulate_stone(stone, runs);
    }
    return result;
}

TEST_CASE("Sample")
{
    auto stones = read_stones("125 17");
    SUBCASE("Part 1")
    {
        auto r = simulate_2(stones, 25);
        //simulate(stones, 25);
        //CHECK(r == stones.size());
        //CHECK(stones.size() == 55312);
        CHECK(r == 55312);
    }
    SUBCASE("Part 2")
    {
        auto r = simulate_2(stones, 75);
        CHECK(r == 65601038650482);
    }
}

TEST_CASE("Input")
{
    auto stones = read_stones("3935565 31753 437818 7697 5 38 0 123");
    SUBCASE("Part 1")
    {
        auto r = simulate_2(stones, 25);
        //simulate(stones, 25);
        //CHECK(r == stones.size());
        //CHECK(stones.size() == 207683);
        CHECK(r == 207683);
    }
    SUBCASE("Part 2")
    {
        auto r = simulate_2(stones, 75);
        CHECK(r == 244782991106220);
    }
}
