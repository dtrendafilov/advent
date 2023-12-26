#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"


typedef std::vector<std::string> Map;

Map read_map(const char* input_name)
{
    Map the_map;
    std::ifstream input(input_name);
    std::string line;
    while (std::getline(input, line))
    {
        the_map.push_back(line);
    }
    return the_map;
}

struct Position
{
    uint64_t col;
    uint64_t row;
};

class Universe
{
public:
    Universe(Map map)
        : _map(map)
    {
        for (uint64_t row = 0; row < _map.size(); ++row)
        {
            bool empty_row = true;
            for (uint64_t col = 0; col < _map[row].size(); ++col)
            {
                if (_map[row][col] == '#')
                {
                    empty_row = false;
                    _galaxies.push_back({col, row});
                }
            }
            if (empty_row)
            {
                _empty_rows.push_back(row);
            }
        }

        {
            std::vector<uint64_t> all_cols(_map[0].size());
            std::iota(all_cols.begin(), all_cols.end(), 0);
            std::set<uint64_t> used_cols;
            for (auto& galaxy : _galaxies)
            {
                used_cols.insert(galaxy.col);
            }
            std::set_difference(all_cols.begin(), all_cols.end(),
                                used_cols.begin(), used_cols.end(),
                                std::back_inserter(_empty_cols));
        }
        // Add sentinel values to avoid dealing with end iterators later
        _empty_rows.push_back(_map.size());
        _empty_cols.push_back(_map[0].size());
    }

    uint64_t distance(Position from, Position to, int age) const
    {
        auto from_col_e = std::lower_bound(_empty_cols.begin(), _empty_cols.end(), from.col);
        auto from_row_e = std::lower_bound(_empty_rows.begin(), _empty_rows.end(), from.row);
        auto to_col_e = std::lower_bound(_empty_cols.begin(), _empty_cols.end(), to.col);
        auto to_row_e = std::lower_bound(_empty_rows.begin(), _empty_rows.end(), to.row);

        uint64_t distance = 0;

        auto age_multiplier = age - 1;

        if (from.col <= to.col)
        {
            distance += to.col - from.col + age_multiplier * std::distance(from_col_e, to_col_e);
        }
        else
        {
            distance += from.col - to.col + age_multiplier * std::distance(to_col_e, from_col_e);
        }

        if (from.row <= to.row)
        {
            distance += to.row - from.row + age_multiplier * std::distance(from_row_e, to_row_e);
        }
        else
        {
            distance += from.row - to.row + age_multiplier * std::distance(to_row_e, from_row_e);
        }
        return distance;
    }

    uint64_t all_distances(int age) const
    {
        uint64_t total_distance = 0;
        for (uint64_t i = 0; i < _galaxies.size(); ++i)
        {
            for (uint64_t j = i + 1; j < _galaxies.size(); ++j)
            {
                total_distance += distance(_galaxies[i], _galaxies[j], age);
            }
        }
        return total_distance;
    }

    bool is_galaxy(Position pos) const
    {
        return _map[pos.row][pos.col] == '#';
    }

    void print() const
    {
        std::cout << std::endl;
        for (auto& row : _map)
        {
            std::cout << row << std::endl;
        }
        std::cout << "rows:";
        for (auto row : _empty_rows)
        {
            std::cout << ' ' << row;
        }
        std::cout << std::endl;

        std::cout << "cols:";
        for (auto col : _empty_cols)
        {
            std::cout << ' ' << col;
        }
        std::cout << std::endl;
    }

    typedef std::vector<Position> Galaxies;
private:
    Map _map;
    typedef std::vector<uint64_t> EmptySpaces;
    Galaxies _galaxies;
    EmptySpaces _empty_rows;
    EmptySpaces _empty_cols;
};



TEST_CASE("Sample")
{
    auto the_map = read_map("sample.txt");
    Universe universe(the_map);
    SUBCASE("Part 1")
    {
        CHECK(universe.is_galaxy({3, 0}));
        CHECK(universe.is_galaxy({7, 8}));
        CHECK(universe.distance({3, 0}, {7, 8}, 2) == 15);        
        CHECK(universe.all_distances(2) == 374);
    }
    SUBCASE("Part 2")
    {
        CHECK(universe.all_distances(10) == 1030);
        CHECK(universe.all_distances(100) == 8410);
    }
}

TEST_CASE("Input")
{
    auto the_map = read_map("input.txt");
    Universe universe(the_map);
    SUBCASE("Part 1")
    {
        auto result = universe.all_distances(2);
        std::cout << "Part 1: " << result << std::endl;
        CHECK(result == 10231178);
    }
    SUBCASE("Part 2")
    {
        auto result = universe.all_distances(1000000);
        std::cout << "Part 2: " << result << std::endl;
        CHECK(result == 622120986954);
    }
}
