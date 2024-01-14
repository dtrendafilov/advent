#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

typedef std::vector<int> RunLengths;

struct Record
{
    std::string states;
    RunLengths runs;
};

RunLengths RunsFromString(std::string_view numbers)
{
    RunLengths runs;
    auto first = &numbers[0];
    auto last = first + numbers.length();
    while (first != last)
    {
        int number = 0;
        auto r = std::from_chars(first, last, number);
        runs.push_back(number);
        first = (r.ptr != last)? r.ptr + 1 : r.ptr;
    }
    return runs;
}

std::istream& operator>>(std::istream& input, Record& record)
{
    input >> record.states;
    std::string numbers;
    input >> numbers;
    record.runs = RunsFromString(numbers);
    return input;
}

TEST_CASE("Runs")
{
    {
        auto run = RunsFromString("1,2");
        CHECK(run.size() == 2);
        CHECK(run[0] == 1);
        CHECK(run[1] == 2);
        CHECK(RunsFromString("1,2") == RunLengths{1, 2});
    }
}


TEST_CASE("Sample")
{
    std::ifstream input("sample.txt");
    SUBCASE("Part 1")
    {
        for (auto& rec : std::views::istream<Record>(input))
        {
            std::cout << rec.states << ' ' << rec.runs.size() << std::endl;
        }
    }
    SUBCASE("Part 2")
    {
    }
}

TEST_CASE("Input")
{
    SUBCASE("Part 1")
    {
    }
    SUBCASE("Part 2")
    {
    }
}
