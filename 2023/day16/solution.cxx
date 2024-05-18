#include "precompiled.hxx"
#include <algorithm>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

typedef std::vector<std::string> Map;

Map read_input(const char* name)
{
    std::fstream input(name);

    Map map;
    std::string line;
    map.push_back(line);
    while (std::getline(input, line))
    {
        map.emplace_back('#' + line + '#');
    }
    map.emplace_back(std::string(map.back().size(), '#'));
    map[0] = std::string(map.back().size(), '#');

    return map;
}

enum Direction
{
    None = 0,
    Left = 1,
    Right = 2,
    Up = 4,
    Down = 8,
};

struct Beam
{
    int x;
    int y;
    Direction dir;
};

typedef std::vector<std::vector<int>> Lightmap;
typedef std::queue<Beam> Beams;

bool light(const Map& map, Beam& beam, Lightmap& lightmap, Beams& beams)
{
    if ((lightmap[beam.y][beam.x] & beam.dir) || map[beam.y][beam.x] == '#')
    {
        return false;
    }
    lightmap[beam.y][beam.x] |= beam.dir;
    switch (beam.dir)
    {
        case Direction::Left:
        {
            switch (map[beam.y][beam.x])
            {
                case '/': {
                              beam.dir = Direction::Down;
                              break;
                          }
                case '\\':{
                              beam.dir = Direction::Up;
                              break;
                          }
                case '|': {
                              beam.dir = Direction::Up;
                              beams.push(Beam{beam.x, beam.y, Direction::Down});
                              break;
                          }
            }
            break;
        }
        case Direction::Right:
        {
            switch (map[beam.y][beam.x])
            {
                case '/': {
                              beam.dir = Direction::Up;
                              break;
                          }
                case '\\':{
                              beam.dir = Direction::Down;
                              break;
                          }
                case '|': {
                              beam.dir = Direction::Up;
                              beams.push(Beam{beam.x, beam.y, Direction::Down});
                              break;
                          }
            }
            break;
        }
        case Direction::Up:
        {
            switch (map[beam.y][beam.x])
            {
                case '/': {
                              beam.dir = Direction::Right;
                              break;
                          }
                case '\\': {
                              beam.dir = Direction::Left;
                              break;
                          }
                case '-': {
                              beam.dir = Direction::Left;
                              beams.push(Beam{beam.x, beam.y, Direction::Right});
                              break;
                          }
            }
            break;
        }
        case Direction::Down:
        {
            switch (map[beam.y][beam.x])
            {
                case '/': {
                              beam.dir = Direction::Left;
                              break;
                          }
                case '\\': {
                              beam.dir = Direction::Right;
                              break;
                          }
                case '-': {
                              beam.dir = Direction::Left;
                              beams.push(Beam{beam.x, beam.y, Direction::Right});
                              break;
                          }
            }
            break;
        }
        case Direction::None: break;
    }
    switch (beam.dir)
    {
        case Direction::Left:  --beam.x; break;
        case Direction::Right: ++beam.x; break;
        case Direction::Up:    --beam.y; break;
        case Direction::Down:  ++beam.y; break;
        case Direction::None: break;
    }
    return true;
}

int lightup(const Map& map, Beam start)
{
    Lightmap visited(map.size());
    for (auto& row : visited)
    {
        row.resize(map[0].size());
    }

    Beams beams;
    beams.push(start);

    while (!beams.empty())
    {
        auto beam = beams.front();
        beams.pop();
        bool move = true;
        while (move) {
            move = light(map, beam, visited, beams);
        }
    }

    int energized = 0;
    int rowSize = int(visited[0].size());
    for (const auto& row : visited)
    {
        energized += rowSize - int(std::count(begin(row), end(row), 0));
    }

    return energized;
}

int all_lights(const Map& map)
{
    int rows = map.size() - 2;
    int cols = map[0].size() - 2;
    std::vector<Beam> beams;
    beams.reserve(2 * rows + 2 * cols);
    for (auto r = 1; r <= rows; ++r)
    {
        beams.emplace_back(Beam{1, r, Direction::Right});
        beams.emplace_back(Beam{cols, r, Direction::Left});
    }
    for (auto c = 1; c <= cols; ++c)
    {
        beams.emplace_back(Beam{c, 1, Direction::Down});
        beams.emplace_back(Beam{c, rows, Direction::Up});
    }

    std::vector<int> lights(beams.size());
    std::transform(std::execution::par, begin(beams), end(beams), begin(lights),
            [&map](const Beam& start) { return lightup(map, start); });

    return *std::max_element(begin(lights), end(lights));
}


TEST_CASE("Sample")
{
    auto map = read_input("sample.txt");
    SUBCASE("Part 1")
    {
        CHECK(lightup(map, Beam{1, 1, Direction::Right}) == 46);
    }
    SUBCASE("Part 2")
    {
        CHECK(all_lights(map) == 51);
    }
}

TEST_CASE("Input")
{
    auto map = read_input("input.txt");
    SUBCASE("Part 1")
    {
        CHECK(lightup(map, Beam{1, 1, Direction::Right}) == 6514);
    }
    SUBCASE("Part 2")
    {
        CHECK(all_lights(map) == 8089);
    }
}
