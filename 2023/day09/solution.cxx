#include <iostream>
#include <fstream>
#include <algorithm>
#include <charconv>
#include <numeric>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

typedef int Number;
typedef std::vector<int> Numbers;

std::vector<Numbers> read_input(const char* input_name)
{
    std::vector<Numbers> result;
    std::ifstream input(input_name);
    std::string line;

    while (getline(input, line))
    {
        const char* start = &line.front();
        const char* end = start + line.size();

        Numbers numbers;
        while (start != end)
        {
            Number n = 0;
            auto result = std::from_chars(start, end, n);
            numbers.push_back(n);
            start = result.ptr;
            while (start < end && *start == ' ')
            {
                ++start;
            }
        }
        result.emplace_back(std::move(numbers));
    }

    return result;
}

void print(const Numbers& numbers)
{
    for (auto& n : numbers)
    {
        std::cout << n << ' ';
    }
    std::cout << std::endl;
}

Number compute_next_number(const Numbers& numbers)
{
    Numbers diffs = numbers;
    auto current_begin = begin(diffs);
    auto numbers_end = end(diffs);
    while (current_begin != numbers_end)
    {
        std::adjacent_difference(current_begin, numbers_end, current_begin);
        // keep the last number that we'll need to sum up later
        // [last_diff1, last_diff2, last_diff3, current_diff1, current_diff2]
        //                                      ^
        //                                      |
        //                                      current_begin
        *current_begin = diffs.back();
        ++current_begin;
        if (std::all_of(current_begin, numbers_end,
                    [](Number n) { return n == 0; }))
        {
            return std::accumulate(begin(diffs), current_begin, numbers.back());
        }
    }
    return 0;
}

Number compute_prev_number(const Numbers& numbers)
{
    Numbers diffs = numbers;
    auto current_begin = begin(diffs);
    auto numbers_end = end(diffs);
    while (current_begin != numbers_end)
    {
        std::adjacent_difference(current_begin, numbers_end, current_begin);
        ++current_begin;
        if (std::all_of(current_begin, numbers_end,
                    [](Number n) { return n == 0; }))
        {
            while (current_begin != begin(diffs))
            {
                auto previous = current_begin - 1;
                *previous -= *current_begin;
                current_begin = previous;
            }
            return diffs.front();
        }
    }
    return 0;
}

TEST_CASE("test next")
{
    CHECK(compute_next_number({1, 2, 3}) == 4);
    CHECK(compute_next_number({6, 4, 2}) == 0);
    CHECK(compute_next_number({2, 2}) == 2);

    CHECK(compute_next_number({0, 3, 6, 9, 12, 15}) == 18);
    CHECK(compute_next_number({1, 3, 6, 10, 15, 21}) == 28);
    CHECK(compute_next_number({10, 13, 16, 21, 30, 45}) == 68);
}

TEST_CASE("test previous")
{
    CHECK(compute_prev_number({1, 2, 3}) == 0);
    CHECK(compute_prev_number({6, 4, 2}) == 8);
    CHECK(compute_prev_number({2, 2}) == 2);

    CHECK(compute_prev_number({0, 3, 6, 9, 12, 15}) == -3);
    CHECK(compute_prev_number({1, 3, 6, 10, 15, 21}) == 0);
    CHECK(compute_prev_number({10, 13, 16, 21, 30, 45}) == 5);
}


Number sum_nexts(const std::vector<Numbers>& sequences)
{
    Number sum = 0;
    for (auto seq: sequences)
    {
        sum += compute_next_number(seq);
    }
    return sum;
}

Number sum_prevs(const std::vector<Numbers>& sequences)
{
    Number sum = 0;
    for (auto seq: sequences)
    {
        sum += compute_prev_number(seq);
    }
    return sum;
}


TEST_CASE("Sample 1")
{
    auto sequences = read_input("sample.txt");
    SUBCASE("Part 1")
    {
        CHECK(sum_nexts(sequences) == 114);
    }

    SUBCASE("Part 2")
    {
        CHECK(sum_prevs(sequences) == 2);
    }
}

TEST_CASE("Input")
{
    auto sequences = read_input("input.txt");

    SUBCASE("Part 1")
    {
        auto sum = sum_nexts(sequences);
        std::cout << "part 1: " << sum << std::endl;
    }
    SUBCASE("Part 2")
    {
        auto sum = sum_prevs(sequences);
        std::cout << "part 2: " << sum << std::endl;
    }
}
