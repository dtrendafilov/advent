#include "precompiled.hxx"
#include <numeric>
#include <tuple>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

enum class Tile : char
{
    Empty = '.',
    Obstacle = '#',
    Border = '*',
    Visited = '@',
};
static_assert(sizeof(Tile) == sizeof(char), "Tile is too large");

bool operator==(Tile lhs, char rhs) {
    return char(lhs) == rhs;
}


using Map = std::vector<std::string>;

std::vector<std::string> read_file(const char* name)
{
    const char border = '*';
    std::vector<std::string> lines;
    std::ifstream file(name);
    std::string line;
    
    std::getline(file, line);
    size_t max_length = line.length() + 2;  // +2 for the '#' borders
    
    lines.push_back(std::string(max_length, border));
    
    do {
        line = border + line + border;
        lines.push_back(line);
    } while (std::getline(file, line));
    
    lines.push_back(std::string(max_length, border));
    return lines;
}

struct Point
{
    int y;
    int x;

    const Point& operator+=(const Point& rhs)
    {
        y += rhs.y;
        x += rhs.x;
        return *this;
    }

    Point operator+(const Point& rhs)
    {
        Point r{*this};
        return r += rhs;
    }

    bool operator==(const Point& rhs) const
    {
        return y == rhs.y && x == rhs.x;
    }
};


auto starting(const Map& map)
{
    const int rows = map.size() - 1; 
    const int cols = map.front().size() - 1; 
    for (int y = 1; y < rows; ++y)
    {
        for (int x = 1; x < cols; ++x)
        {
            switch (map[y][x])
            {
                case 'v':
                    return std::make_tuple(Point{y, x}, Point{ 1, 0});
                case '^':
                    return std::make_tuple(Point{y, x}, Point{-1, 0});
                case '<':
                    return std::make_tuple(Point{y, x}, Point{0, -1});
                case '>':
                    return std::make_tuple(Point{y, x}, Point{0,  1});
            }
        }
    }
    return std::make_tuple(Point{0, 0}, Point{0, 0});
}

template <typename E = Tile>
class MapWithPoints
{
public:
    explicit MapWithPoints(Map& map)
        : _map(map)
    {}

    E operator[](Point p) const
    {
        return E{_map[p.y][p.x]};
    }
    E& operator[](Point p)
    {
        return reinterpret_cast<E&>(_map[p.y][p.x]);
    }

    operator const Map&() const
    {
        return _map;
    }


private:
    Map& _map;
};

Point turn_right(Point d)
{
    return Point{d.x, -d.y};
}

void print(const Map& map)
{
    for (auto& r : map)
    {
        std::cout << r << std::endl;
    }
}

Point move(const auto& map, Point position, Point& dir)
{
    auto next = position + dir;
    while (map[next] == Tile::Obstacle)
    {
        dir = turn_right(dir);
        next = position + dir;
    }
    return next;
}

int guard(Map& _map)
{
    MapWithPoints map(_map);
    auto [start, dir] = starting(map);


    do {
        map[start] = Tile::Visited;
        start = move(map, start, dir);
    } while (map[start] != Tile::Border);


    int s = 0;
    for (auto& r : _map)
    {
        s += std::count(begin(r), end(r), char(Tile::Visited));
    }
    return s;
}

char mask(Point d)
{
    auto m = [](int d) { return (d < 0)? 1 : 2 * d; };
    return 4 * m(d.y) + m(d.x);
}

char visited(char current, Point d)
{
    return ((current != Tile::Empty)? current : char(Tile::Visited)) | mask(d);
}

bool is_loop(char current, Point d)
{
    return (((current != Tile::Empty)? current : 0) & mask(d)) == mask(d);
}

bool will_loop(Map& _map, Point start, Point dir)
{
    MapWithPoints<char> map(_map);
    do {
        if (is_loop(map[start], dir))
        {
            return true;
        }
        map[start] = visited(map[start], dir);
        start = move(map, start, dir);
    } while (map[start] != Tile::Border);

    return false;
}

int loops(Map& _map)
{
    MapWithPoints<char> map(_map);
    auto [start, dir] = starting(map);
    map[start] = char(Tile::Empty);
    int l = 0;

    do {
        map[start] = visited(map[start], dir);

        auto pdir = dir;
        auto ppos = move(map, start, pdir);
        if (map[ppos] == Tile::Empty)
        {
            auto _obstructed = _map;
            MapWithPoints obstructed(_obstructed);
            obstructed[ppos] = Tile::Obstacle;
            auto ldir = dir;
            auto lpos = move(obstructed, start, ldir);
            if (will_loop(_obstructed, lpos, ldir))
            {
                ++l;
            }
        }


        start = move(map, start, dir);
    } while (map[start] != char(Tile::Border));

    return l;
}

TEST_CASE("Turn Right")
{
    CHECK(turn_right(Point{ 1,  0}) == Point{ 0, -1});
    CHECK(turn_right(Point{-1,  0}) == Point{ 0,  1});
    CHECK(turn_right(Point{ 0,  1}) == Point{ 1,  0});
    CHECK(turn_right(Point{ 0, -1}) == Point{-1,  0});
}


TEST_CASE("Sample")
{
    SUBCASE("Part 1")
    {
        auto map = read_file("sample.txt");
        CHECK(guard(map) == 41);

    }
    SUBCASE("Part 2")
    {
        auto map = read_file("sample.txt");
        CHECK(loops(map) == 6);
    }
}

TEST_CASE("Input")
{
    SUBCASE("Part 1")
    {
        auto map = read_file("input.txt");
        CHECK(guard(map) == 4663);
    }
    SUBCASE("Part 2")
    {
        auto map = read_file("input.txt");
        auto l = loops(map);
        CHECK(l > 1494);
        CHECK(l < 3644);
        CHECK(l != 1692);
        CHECK(l == 1530);
    }
}
