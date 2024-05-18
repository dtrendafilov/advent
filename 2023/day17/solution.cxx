#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

typedef std::vector<int> Row;
typedef std::vector<Row> Map;


struct Graph
{
    void load(const char* name);

    int shortest();
    int shortest2();

    Map _map;
};

void Graph::load(const char* name)
{
    std::ifstream input(name);
    std::string line;

    while (std::getline(input, line))
    {
        Row r(line.size());
        std::transform(begin(line), end(line), begin(r), [](char cost) {
                return cost - '0';
        });
        _map.emplace_back(std::move(r));
    }
}

enum Direction : uint16_t
{
    Right,
    Down,
    Left,
    Up,
};

struct Vertex
{
    int16_t x;
    int16_t y;
    uint16_t steps;
    Direction direction;

    bool operator==(const Vertex& rhs) const
    {
        return x == rhs.x && y == rhs.y && steps == rhs.steps && direction == rhs.direction;
    }
};

std::ostream& operator<<(std::ostream& out, const Vertex& v)
{
    return out << v.x << ' ' << v.y << ' ' << v.steps << ' ' << v.direction;
}

namespace std
{
    template <>
    struct hash<Vertex>
    {
        size_t operator()(const Vertex& v) const
        {
            std::hash<int64_t> hi64;
            static_assert(sizeof(Vertex) == sizeof(int64_t));
            return hi64(reinterpret_cast<const int64_t&>(v));
        }
    };
}

struct Position
{
    Vertex vertex;
    int distance;

    bool operator<(const Position& rhs) const
    {
        return distance > rhs.distance;
    }
};

std::ostream& operator<<(std::ostream& out, const Position& p)
{
    return out << p.vertex << '@' << p.distance;
}

typedef std::unordered_set<Vertex> Visited;

struct Edge
{
    int dx;
    int dy;
    Direction opposite;
    Direction direction;
};


int Graph::shortest()
{
    std::priority_queue<Position> positions;
    Position start{Vertex{0, 0, 1, Direction::Right}, 0};
    positions.push(start);
    Visited visited;

    int rows = _map.size();
    int cols = _map[0].size();

    Vertex goal{int16_t(cols - 1), int16_t(rows - 1), 0, Direction::Right};

    Edge edges[4] = {
        {-1,  0, Direction::Right, Direction::Left},
        { 1,  0, Direction::Left, Direction::Right},
        { 0, -1, Direction::Down, Direction::Up},
        { 0,  1, Direction::Up, Direction::Down},
    };

    while (!positions.empty())
    {
        auto position = positions.top();
        positions.pop();

        auto& vertex = position.vertex;
        if (!visited.insert(vertex).second)
        {
            continue;
        }

        if (vertex.x == goal.x && vertex.y == goal.y)
        {
            return position.distance;
        }

        for (auto edge: edges)
        {
            if (vertex.direction == edge.opposite)
            {
                continue;
            }
            if (vertex.direction == edge.direction && vertex.steps == 3)
            {
                continue;
            }

            const uint16_t steps = (vertex.direction == edge.direction)? vertex.steps + 1 : 1;
            Vertex next{int16_t(vertex.x + edge.dx), int16_t(vertex.y + edge.dy), steps, edge.direction};

            if (!(0 <= next.x && next.x < cols && 0 <= next.y && next.y < rows))
            {
                continue;
            }

            Position next_position{next, position.distance + _map[next.y][next.x]};
            if (visited.find(next) == visited.end())
            {
                positions.push(next_position);
            }
        }
    }
    return 0;
}

int Graph::shortest2()
{
    int rows = _map.size();
    int cols = _map[0].size();

    Visited visited;
    std::priority_queue<Position> positions;
    {
        int distance = 0;//_map[0][0];
        for (int block = 1; block <= 10 && block < cols; ++block)
        {
            distance += _map[0][block];
            if (block < 4)
            {
                continue;
            }
            Vertex next{int16_t(block), 0, 1, Direction::Right};
            positions.push(Position{next, distance});
        }
    }



    Vertex goal{int16_t(cols - 1), int16_t(rows - 1), 0, Direction::Right};

    Edge edges[4] = {
        {-1,  0, Direction::Right, Direction::Left},
        { 1,  0, Direction::Left, Direction::Right},
        { 0, -1, Direction::Down, Direction::Up},
        { 0,  1, Direction::Up, Direction::Down},
    };

    while (!positions.empty())
    {
        auto position = positions.top();
        positions.pop();

        auto& vertex = position.vertex;
        if (vertex.x == goal.x && vertex.y == goal.y)
        {
            return position.distance;
        }
        if (!visited.insert(vertex).second)
        {
            continue;
        }

        for (auto edge: edges)
        {
            if (vertex.direction == edge.opposite || vertex.direction == edge.direction)
            {
                continue;
            }

            int distance = 0;
            for (int blocks = 1; blocks <= 10; ++blocks)
            {
                Vertex next{int16_t(vertex.x + edge.dx * blocks),
                            int16_t(vertex.y + edge.dy * blocks),
                            0, edge.direction};

                if (!(0 <= next.x && next.x < cols && 0 <= next.y && next.y < rows))
                {
                    continue;
                }
                distance += _map[next.y][next.x];
                if (blocks < 4)
                {
                    continue;
                }

                Position next_position{next, position.distance + distance};
                if (visited.find(next) == visited.end())
                {
                    positions.push(next_position);
                }
            }
        }
    }
    return 0;
}


TEST_CASE("Sample")
{
    Graph graph;
    graph.load("sample.txt");

    SUBCASE("Part 1")
    {
        CHECK(graph.shortest() == 102);
    }
    SUBCASE("Part 2")
    {
        CHECK(graph.shortest2() == 94);
    }
}

TEST_CASE("Input")
{
    Graph graph;
    graph.load("input.txt");
    SUBCASE("Part 1")
    {
        CHECK(graph.shortest() == 956);
    }
    SUBCASE("Part 2")
    {
        CHECK(graph.shortest2() == 1106);
    }
}
