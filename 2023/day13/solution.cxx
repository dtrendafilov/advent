#include "precompiled.hxx"
#include <string_view>
#include <cassert>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"


typedef std::vector<std::string> Map;
typedef std::vector<Map> Maps;


Maps read_input(const char* name)
{
    std::ifstream input(name);

    std::string line;

    Maps maps(1);
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            maps.back().push_back(line);
        }
        else
        {
            maps.emplace_back(Map{});
        }
    }
    return maps;
}

Map transpose(const Map& original)
{
    const auto rows = original[0].size();
    const auto cols = original.size();
    Map transposed(original[0].size());
    for (auto r = 0u; r < rows; ++r)
    {
        auto& row = transposed[r];
        row.resize(cols);
        for (auto c = 0u; c < cols; ++c)
        {
            row[c] = original[c][r];
        }
    }
    return transposed;
}

bool is_mirror(const Map& map, size_t row)
{
    auto rows = std::min(row + 1, map.size() - row - 1);
    auto second_begin = begin(map) + row + 1;
    auto first_begin = rbegin(map) + (map.size() - row - 1);
    return std::equal(second_begin, second_begin + rows,
            first_begin);
}

template <typename Check>
int find_mirror(const Map& map, Check check)
{
    const auto rows = map.size() - 1;
    for (auto r = 0u; r < rows; ++r)
    {
        if (check(map, r))
        {
            return r;
        }
    }
    return -1;
}

template <typename Check>
struct Score
{
    int score(const Map& map)
    {
        auto row = find_mirror(map, m_check);
        if (row == -1)
        {
            auto transposed = transpose(map);
            return find_mirror(transposed, m_check) + 1;
        }
        return (row + 1) * 100;
    }
    Check m_check;
};


int difference(std::string_view l, std::string_view r)
{
    assert(l.size() == r.size());
    return std::inner_product(begin(l), end(l), begin(r), 0, std::plus<int>{},
            [](char lc, char rc)
            {
                return std::abs(lc - rc);
            });
}

bool is_mirror_smudge(const Map& map, size_t row)
{
    auto rows = std::min(row + 1, map.size() - row - 1);
    auto second_begin = begin(map) + row + 1;
    auto second_end = second_begin + rows;
    auto first_begin = rbegin(map) + (map.size() - row - 1);
    int diff =  std::inner_product(second_begin, second_end, first_begin, 0,
            std::plus<int>{}, difference);
    return diff == std::abs('#' - '.');
}


TEST_CASE("Sample")
{
    auto maps = read_input("sample.txt");
    SUBCASE("Part 1")
    {
        CHECK(find_mirror(maps[0], is_mirror) == -1);
        CHECK(find_mirror(transpose(maps[0]), is_mirror) == 4);
        CHECK(find_mirror(maps[1], is_mirror) == 3);
        CHECK(maps.size() == 2);

        auto sum = 0;
        auto scorer = Score(is_mirror);
        for (const auto& m : maps)
        {
            sum += scorer.score(m);
        }
        CHECK(sum == 405);
    }
    SUBCASE("Part 2")
    {
        auto sum = 0;
        auto scorer = Score(is_mirror_smudge);
        for (const auto& m : maps)
        {
            sum += scorer.score(m);
        }
        CHECK(sum == 400);
    }
}

TEST_CASE("Input")
{
    auto maps = read_input("input.txt");
    SUBCASE("Part 1")
    {
        auto sum = 0;
        auto scorer = Score(is_mirror);
        for (const auto& m : maps)
        {
            sum += scorer.score(m);
        }
        CHECK(sum == 34918);
    }
    SUBCASE("Part 2")
    {
        auto sum = 0;
        auto scorer = Score(is_mirror_smudge);
        for (const auto& m : maps)
        {
            sum += scorer.score(m);
        }
        CHECK(sum == 33054);
    }
}
