#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <cstdlib>
#include <cassert>
#include <ranges>
#include <numeric>


typedef std::vector<std::string> Map;

enum Tile : char
{
    Empty = ' ',
    Free  = '.',
    Wall  = '#',
};

enum Orientation : int
{
    Right,
    Down,
    Left,
    Up,
};

struct Position
{
    int Row;
    int Col;
    Orientation Facing;
};

std::ostream& operator<<(std::ostream& stream, const Position& position)
{
    return stream << position.Row << ':' << position.Col << '@' << position.Facing;
}

auto read(const char* input_name)
{
    Map map(1); // start with sentinel
    std::ifstream input(input_name);
    std::string line;
    while (std::getline(input, line) && !line.empty())
    {
        std::string guarded;
        guarded.reserve(2 + line.size());
        guarded.push_back(Tile::Empty);
        guarded.insert(guarded.end(), line.begin(), line.end());
        guarded.push_back(Tile::Empty);
        map.push_back(guarded);

    }
    // end with sentinel
    map.push_back(line);

    auto lengths = std::views::transform(map, [](auto& l) { return l.size(); });
    // TODO: use ranges::fold when available
    auto max_length = std::accumulate(begin(lengths), end(lengths), 0,
        [](size_t l, size_t r) {
            return std::max(l, r);
    });

    for (auto& row: map)
    {
        row.resize(max_length, Tile::Empty);
    }

    // commands
    std::getline(input, line);

    return std::make_pair(map, line);
}

Position get_starting_position(const Map& map)
{
    Position start {1, 0, Orientation::Right};
    auto first_free = std::find(map[1].begin(), map[1].end(), Tile::Free);
    start.Col = std::distance(map[1].begin(), first_free);
    assert(size_t(start.Col) < map[1].size());
    return start;
}


int main(int argc, const char* argv[])
{
    auto input = (argc > 1)? argv[1] : "input.txt";
    auto [map, commands] = read(input);
    auto position = get_starting_position(map);

#if 0
    std::cout << start << std::endl;
    for (const auto& row : map)
    {
        std::cout << row << std::endl;
    }
#endif

    // Part 1
    {
        auto password =
            1000 * position.Row + 4 * position.Col + int(position.Facing);
        std::cout << password << std::endl;
    }

    // Part 2
    {
    }

    return 0;
}
