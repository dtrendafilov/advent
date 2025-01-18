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

bool is_safe_iterator(auto begin, auto next, auto end, bool allow_skip = false)
{
    if (next == end)
    {
        return true;
    }
    int sign = (*begin - *next) > 0? 3 : -3;
    while (next != end)
    {
        auto diff = (*begin - *next) * sign;
        if (diff <= 0 || diff > 9)
        {
            if (allow_skip)
            {
                allow_skip = false;
                ++next;
            }
            else
            {
                return false;
            }
        }
        else
        {
            begin = next++;
        }
    }
    return true;
}

bool is_safe_dampen(const std::vector<int>& report)
{
    auto b = begin(report);
    auto e = end(report);
    if (b == e || b + 1 == e)
        return true;

    CHECK(is_safe(report) == is_safe_iterator(b, b + 1, e));

    if (is_safe_iterator(b, b + 1, e))
        return true;
    if (is_safe_iterator(b + 1, b + 2, e))
        return true;
    if (is_safe_iterator(b, b + 2, e))
        return true;
    return is_safe_iterator(b, b + 1, e, true);
}

int safe_reports(const std::vector<std::vector<int>>& reports)
{
    return std::count_if(reports.begin(), reports.end(), is_safe);
}

int safe_reports_dampen(const std::vector<std::vector<int>>& reports)
{
    return std::count_if(reports.begin(), reports.end(), is_safe_dampen);
}


TEST_CASE("Sample")
{
    auto reports = read_file("sample.txt");
    SUBCASE("Part 1")
    {
        CHECK(safe_reports(reports) == 2);
    }
    SUBCASE("Part 2")
    {
        CHECK(safe_reports_dampen(reports) == 4);
    }
}

TEST_CASE("Input")
{
    auto reports = read_file("input.txt");
    SUBCASE("Part 1")
    {
        CHECK(safe_reports(reports) == 369);
    }
    SUBCASE("Part 2")
    {
        CHECK(safe_reports_dampen(reports) == 428);
    }
}
