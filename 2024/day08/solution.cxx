#include "precompiled.hxx"
#include <compare>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

struct Point
{
    int x;
    int y;

    std::strong_ordering operator <=>(const Point& rhs) const = default;
};

struct PointHash
{
    size_t operator()(const Point& p) const
    {
        uint64_t h = p.x;
        h <<= 32;
        h |= p.y;
        return std::hash<uint64_t>{}(h);
    }
};

using Map = std::vector<std::string>;

Map read_input(const char* name)
{
    std::ifstream input(name);
    std::string line;

    Map m;

    while (std::getline(input, line))
    {
        m.emplace_back(line);
    }
    return m;
}

using Antennas = std::unordered_map<char, std::vector<Point>>;

Antennas get_antennas(const Map& m)
{
    Antennas a;

    const int rows = m.size();
    const int cols = m.front().size();

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            char s = m[y][x];
            if (('0' <= s && s <= '9') || ('a' <= s && s <='z') || ('A' <= s && s <= 'Z'))
            {
                a[s].emplace_back(Point{x, y});
            }
        }
    }

    return a;
}


using Antinodes = std::unordered_set<Point, PointHash>;

void add_antinodes(const Point& l, const Point& r, int rows, int cols, bool resonant, Antinodes& antinodes)
{
    int dx = r.x - l.x;
    int dy = r.y - l.y;
    int rx = l.x;
    int ry = l.y;

    if (resonant)
    {
        antinodes.insert(Point{rx, ry});
    }

    do
    {
        rx -= dx;
        ry -= dy;
        if ((0 <= rx && rx < cols) && (0 <= ry && ry < rows))
        {
            antinodes.insert(Point{rx, ry});
        }
        else
        {
            break;
        }
    } while (resonant);
}

int count_antinodes(const Antennas& antennas, const Map& m, bool resonant)
{
    const int rows = m.size();
    const int cols = m.front().size();

    Antinodes antinodes;

    for (auto& [c, positions] : antennas)
    {
        /* std::println("signal {}", c); */
        const auto count = positions.size();
        for (auto i = 0u; i < count; ++i)
        {
            for (auto j = i + 1; j < count; ++j)
            {
                add_antinodes(positions[i], positions[j], rows, cols, resonant, antinodes);
                add_antinodes(positions[j], positions[i], rows, cols, resonant, antinodes);
            }
        }
    }
    

    return antinodes.size();
}


TEST_CASE("Sample")
{
    auto m = read_input("sample.txt");
    auto a = get_antennas(m);
    SUBCASE("Part 1")
    {
        CHECK(count_antinodes(a, m, false) == 14);
    }
    SUBCASE("Part 2")
    {
        CHECK(count_antinodes(a, m, true) == 34);
    }
}

TEST_CASE("Input")
{
    auto m = read_input("input.txt");
    auto a = get_antennas(m);
    SUBCASE("Part 1")
    {
        CHECK(count_antinodes(a, m, false) == 348);
    }
    SUBCASE("Part 2")
    {
        CHECK(count_antinodes(a, m, true) == 34);
    }
}
