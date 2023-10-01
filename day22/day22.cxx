#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
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
    if (line.back() != 'L' || line.back() != 'R')
    {
        line.push_back('.');
    }

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

bool can_move(const Map& map, int row, int column, Position& position)
{
    if (map[row][column] == Tile::Free)
    {
        position.Row = row;
        position.Col = column;
        return true;
    }
    return false;
}

auto wrap_around(const Map& map, const Position& position)
{
    int row = position.Row;
    int column = position.Col;
    switch (position.Facing)
    {
        case Orientation::Up:
            {
                auto row_it = std::find_if(map.rbegin(), map.rend(),
                        [column](auto& map_row) {
                            return map_row[column] != Tile::Empty;
                        });
                row = std::distance(row_it, map.rend());
                break;
            }
        case Orientation::Down:
            {
                auto row_it = std::find_if(map.begin(), map.end(),
                        [column](auto& map_row) {
                            return map_row[column] != Tile::Empty;
                        });
                row = std::distance(map.begin(), row_it);
                break;
            }
        case Orientation::Left:
            column = map[row].find_last_not_of(Tile::Empty);
            break;
        case Orientation::Right:
            column = map[row].find_first_not_of(Tile::Empty);
            break;
    }
    return std::make_pair(row, column);
}

bool step(const Map& map, Position& position)
{
    int row = position.Row;
    int column = position.Col;
    switch (position.Facing)
    {
        case Orientation::Up:
            --row;
            break;
        case Orientation::Down:
            ++row;
            break;
        case Orientation::Left:
            --column;
            break;
        case Orientation::Right:
            ++column;
            break;
    }
    if (map[row][column] == Tile::Empty)
    {
        std::tie(row, column) = wrap_around(map, position);
    }
    return can_move(map, row, column, position);
}

Position next_position(const Map& map, const Position& current, int steps,
        int rotation)
{
    Position position = current;
    while (steps-- && step(map, position))
        ;

    int new_facing = int(position.Facing) + rotation;
    if (new_facing < 0)
    {
        new_facing = Orientation::Up;
    }
    else if (new_facing > Orientation::Up)
    {
        new_facing = Orientation::Right;
    }
    position.Facing = Orientation(new_facing);
    return position;
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
        std::istringstream command_stream(commands);
        int steps = 0;
        int rotation = 0;
        char rotation_command = '.';
        do {
            command_stream >> steps >> rotation_command;
            switch (rotation_command)
            {
                case 'L': rotation = -1; break;
                case 'R': rotation = 1; break;
                case '.': rotation = 0; break;
            }
            position = next_position(map, position, steps, rotation);
            std::cout << position.Row << 'x' << position.Col << '@' << position.Facing << std::endl;
        } while (command_stream);

        auto password =
            1000 * position.Row + 4 * position.Col + int(position.Facing);
        std::cout << password << std::endl;
    }

    // Part 2
    {
    }

    return 0;
}
