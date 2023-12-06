#include <charconv>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <string_view>
#include <vector>
#include <string>


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

TEST_CASE("Answer is sane")
{
    CHECK(6 * 7 == 42);
}

typedef std::vector<uint64_t> Numbers;

template <typename T>
std::vector<T> read_numbers(std::string_view line)
{
    std::vector<T> numbers;
    std::regex integer_re("[[:digit:]]+");
    // decay string_view iterators to const char*
    auto line_start = &line[0];
    auto line_end = line_start + line.length();
    auto it_begin = std::cregex_iterator(line_start, line_end, integer_re);
    auto it_end = std::cregex_iterator();
    std::transform(it_begin, it_end, std::back_inserter(numbers), [](auto m) {
            T number{};
            auto result = std::from_chars(m[0].first, m[1].second, number);
            CHECK(result.ec == std::errc{});
            return number;
    });
    return numbers;
}

TEST_CASE("line to numbers")
{
    SUBCASE("empty")
    {
        auto numbers = read_numbers<int>("");
        CHECK(numbers.empty());
    }
    SUBCASE("single")
    {
        auto numbers = read_numbers<int>("The answer is 42");
        CHECK(numbers == std::vector<int>{42});
    }
    SUBCASE("multi")
    {
        auto numbers = read_numbers<int>("6 * 7 == 42");
        CHECK(numbers == std::vector<int>{6, 7, 42});
    }
}


uint64_t ways_to_win(uint64_t time, uint64_t record)
{
    auto det = std::sqrt(time * time - 4 * record);
    auto lower = std::floor((time - det) / 2);
    auto upper = std::ceil((time + det) / 2);
    CHECK(lower * (time - lower) <= record);
    CHECK(upper * (time - upper) <= record);

    CHECK((lower + 1) * (time - lower - 1) > record);
    CHECK((upper - 1) * (time - upper + 1) > record);

#if 0
    uint64_t wins = 0;
    for (uint64_t h = lower; h <= upper; ++h)
    {
        if ((h * (time - h)) > record)
        {
            ++wins;
        }
    }
    CHECK(wins == uint64_t(upper - lower - 1));
#endif
    return upper - lower - 1;
}

TEST_CASE("count wins")
{
    CHECK(ways_to_win(7, 9) == 4);
    CHECK(ways_to_win(15, 40) == 8);
    CHECK(ways_to_win(30, 200) == 9);
}

void read_book(const char* input_name,
        Numbers& times,
        Numbers& records)
{
    std::fstream input(input_name);
    std::string line;
    std::getline(input, line);
    times = read_numbers<uint64_t>(line);
    std::getline(input, line);
    records = read_numbers<uint64_t>(line);
    CHECK(times.size() == records.size());
}

void read_book2(const char* input_name,
        Numbers& times,
        Numbers& records)
{
    std::fstream input(input_name);
    std::string line;
    std::getline(input, line);
    line.erase(std::remove(begin(line), end(line), ' '), end(line));
    times = read_numbers<uint64_t>(line);
    std::getline(input, line);
    line.erase(std::remove(begin(line), end(line), ' '), end(line));
    records = read_numbers<uint64_t>(line);
    CHECK(times.size() == records.size());
}

uint64_t all_wins(const Numbers& times, const Numbers& records)
{
    uint64_t all = 1;
    for (auto i = 0u; i < times.size(); ++i)
    {
        all *= ways_to_win(times[i], records[i]);
    }
    return all;
}

TEST_CASE("Sample")
{
    Numbers times;
    Numbers records;
    SUBCASE("Part 1")
    {
        read_book("sample.txt", times, records);
        CHECK(all_wins(times, records) == 288);
    }

    SUBCASE("Part 2")
    {
        read_book2("sample.txt", times, records);
        CHECK(all_wins(times, records) == 71503);
    }
}

TEST_CASE("Input")
{
    Numbers times;
    Numbers records;
    SUBCASE("Part 1")
    {
        read_book("input.txt", times, records);
        const auto answer = all_wins(times, records);
        CHECK(answer == 293046);
        std::cout << "part 1: " << answer << std::endl;
    }
    SUBCASE("Part 2")
    {
        read_book2("input.txt", times, records);
        const auto answer = all_wins(times, records);
        CHECK(answer == 35150181);
        std::cout << "part 2: " << answer << std::endl;
    }
}
