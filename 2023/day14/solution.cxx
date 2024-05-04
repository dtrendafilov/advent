#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

typedef std::vector<std::string> Map;


Map read_input(const char* name)
{
    std::ifstream input(name);

    std::string line;

    Map map(1);
    while (std::getline(input, line))
    {
        map.push_back('#' + line + '#');
    }
    map[0] = std::string(map[1].size(), '#');
    map.emplace_back(std::string(map[1].size(), '#'));
    return map;
}

size_t total_weight(const Map& map)
{
    const auto rows = map.size() - 1;
    const auto cols = map[0].size();
    std::vector<size_t> limits(cols, 0);
    size_t weight = 0;
    for (auto row = 1u; row < rows; row++)
    {
        for (auto col = 0u; col < cols; col++)
        {
            switch (map[row][col])
            {
                case 'O': {
                    ++limits[col];
                    weight += rows - limits[col];
                    break;
                }
                case '#': {
                    limits[col] = row;
                    break;
                }
            }
        }
    }
    return weight;
}

void tilt_north(Map& map)
{
    const auto rows = map.size() - 1;
    const auto cols = map[0].size();
    std::vector<size_t> limits(cols, 0);
    for (auto row = 1u; row < rows; row++)
    {
        for (auto col = 0u; col < cols; col++)
        {
            switch (map[row][col])
            {
                case 'O': {
                    ++limits[col];
                    map[row][col] = '.';
                    map[limits[col]][col] = 'O';
                    break;
                }
                case '#': {
                    limits[col] = row;
                    break;
                }
            }
        }
    }
}

void tilt_south(Map& map)
{
    const auto rows = map.size() - 1;
    const auto cols = map[0].size() - 1;
    std::vector<size_t> limits(cols, rows);
    for (auto row = rows - 1; row > 0; row--)
    {
        for (auto col = 1u; col < cols; col++)
        {
            switch (map[row][col])
            {
                case 'O': {
                    --limits[col];
                    map[row][col] = '.';
                    map[limits[col]][col] = 'O';
                    break;
                }
                case '#': {
                    limits[col] = row;
                    break;
                }
            }
        }
    }
}

void tilt_west(Map& map)
{
    const auto rows = map.size() - 1;
    const auto cols = map[0].size() - 1;
    std::vector<size_t> limits(rows, 0);
    for (auto col = 0u; col < cols; ++col)
    {
        for (auto row = 1u; row < rows; row++)
        {
            switch (map[row][col])
            {
                case 'O': {
                    ++limits[row];
                    map[row][col] = '.';
                    map[row][limits[row]] = 'O';
                    break;
                }
                case '#': {
                    limits[row] = col;
                    break;
                }
            }
        }
    }
}

void tilt_east(Map& map)
{
    const auto rows = map.size() - 1;
    const auto cols = map[0].size() - 1;
    std::vector<size_t> limits(rows, cols);
    for (auto col = cols - 1; col > 0; col--)
    {
        for (auto row = 1u; row < rows; row++)
        {
            switch (map[row][col])
            {
                case 'O': {
                    --limits[row];
                    map[row][col] = '.';
                    map[row][limits[row]] = 'O';
                    break;
                }
                case '#': {
                    limits[row] = col;
                    break;
                }
            }
        }
    }
}

void print(const Map& map)
{
    for (const auto& row : map)
    {
        std::cout << row << std::endl;
    }
}


size_t get_weight(const Map& map)
{
    const auto rows = map.size() - 1;
    const auto cols = map[0].size() - 1;
    size_t weight = 0;
    for (auto row = 1u; row < rows; row++)
    {
        for (auto col = 1u; col < cols; col++)
        {
            if (map[row][col] == 'O')
            {
                weight += rows - row;
            }
        }
    }
    return weight;
}

void spin(Map& map, size_t steps)
{
    typedef std::map<Map, size_t> States;
    States states;
    for (size_t i = 1; i <= steps; ++i)
    {
        tilt_north(map);
        tilt_west(map);
        tilt_south(map);    
        tilt_east(map);
        auto [it, inserted] = states.insert({map, i});
        if (!inserted)
        {
            const auto cycle = i - it->second;
            const auto remaining = (steps - i) % cycle;
            for (size_t j = 1; j <= remaining; ++j)
            {
                tilt_north(map);
                tilt_west(map);
                tilt_south(map);    
                tilt_east(map);
            }
            break;
        }
    }
}

TEST_CASE("Sample")
{
    auto map = read_input("sample.txt");
    SUBCASE("Part 1")
    {
        CHECK(total_weight(map) == 136);
    }
    SUBCASE("Part 2")
    {
        spin(map, 1000000000);
        CHECK(get_weight(map) == 64);
    }
}

TEST_CASE("Input")
{
    auto map = read_input("input.txt");
    SUBCASE("Part 1")
    {
        CHECK(total_weight(map) == 108641);
    }
    SUBCASE("Part 2")
    {
        spin(map, 1000000000);
        CHECK(get_weight(map) == 84328);
    }
}
