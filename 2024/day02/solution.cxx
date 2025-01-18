#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

std::vector<std::vector<int>> read_file(const std::string& filename)
{
    std::vector<std::vector<int>> result;
    std::ifstream file(filename);
    std::string line;
    
    while (std::getline(file, line))
    {
        std::vector<int> numbers;
        std::istringstream iss(line);
        int num;
        while (iss >> num)
        {
            numbers.push_back(num);
        }
        result.push_back(numbers);
    }
    return result;
}

bool is_safe(const std::vector<int>& report)
{
    if (report.size() < 2)
    {
        return true;
    }
    auto differences = std::views::adjacent_transform<2>(report, std::minus{});
    int sign = (report[0] - report[1]) > 0 ? 1 : -1;
    for (auto diff : differences)
    {
        if (diff < -3 || diff > 3 || diff == 0)
        {
            return false;
        }
        if (sign * diff < 0)
        {
            return false;
        }
    }
    return true;
}

int safe_reports(const std::vector<std::vector<int>>& reports)
{
    return std::count_if(reports.begin(), reports.end(), is_safe);
}


TEST_CASE("Sample")
{
    SUBCASE("Part 1")
    {
        auto reports = read_file("sample.txt");
        CHECK(safe_reports(reports) == 2);

    }
    SUBCASE("Part 2")
    {
        auto reports = read_file("input.txt");
        CHECK(safe_reports(reports) == 369);
    }
}

TEST_CASE("Input")
{
    SUBCASE("Part 1")
    {
        CHECK(6 * 4 == 24);
    }
    SUBCASE("Part 2")
    {
        CHECK(6 + 18 == 24);
    }
}
