#include "precompiled.hxx"

#include "../darllen.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

using Number = int32_t;

struct Point
{
    Number x;
    Number y;

    std::strong_ordering operator<=>(const Point& other) const = default;
};

std::ostream& operator<<(std::ostream& os, const Point& p) {
    return os << "p(" << p.x << ',' << p.y << ')';
}

struct Velocity
{
    Number x;
    Number y;
    std::strong_ordering operator<=>(const Velocity& other) const = default;
};

std::ostream& operator<<(std::ostream& os, const Velocity& v) {
    return os << "v(" << v.x << ',' << v.y << ')';
}

struct Robot
{
    Point position;
    Velocity velocity;
};

using Robots = std::vector<Robot>;

Robots read_input(const char* name)
{
    Robots robots;
    std::ifstream file(name);
    std::string line;
    
    // Regex for "p=0,4 v=3,-3" format
    std::regex pattern(R"(p=(-?\d+),(-?\d+)\s+v=(-?\d+),(-?\d+))");
    
    while (std::getline(file, line)) {
        Number px, py, vx, vy;
        if (darllen::read(pattern, line.begin(), line.end(),
                         px, py, vx, vy)) {
            robots.push_back({{px, py}, {vx, vy}});
        }
    }
    return robots;
}

template <Number COLUMNS, Number ROWS>
Point simulate(const Robot& robot, Number steps)
{
    Point position = robot.position;
    for (Number i = 0; i < steps; ++i)
    {
        position.x = (COLUMNS + position.x + robot.velocity.x) % COLUMNS;
        position.y = (ROWS + position.y + robot.velocity.y) % ROWS;
    }
    return position;
}

constexpr Number modulo(Number a, Number b)
{
    const auto m = a % b;
    return m >= 0 ? m : m + b;
}

template <Number COLUMNS, Number ROWS>
Point teleport(const Robot& robot, Number steps)
{
    Point position {modulo(robot.position.x + robot.velocity.x * steps, COLUMNS),
                    modulo(robot.position.y + robot.velocity.y * steps, ROWS)};
    return position;
}

TEST_CASE("Modulo")
{
    constexpr Number ROWS = 103;

    CHECK(-10 % ROWS != ROWS - 10);
    CHECK(-150 % ROWS == -150 + ROWS);
    CHECK(modulo(-150, ROWS) == 2 * ROWS - 150);
    CHECK(modulo(ROWS, ROWS) == 0);
}

template <Number COLUMNS, Number ROWS>
Number safety(const Robots& robots)
{
    constexpr Number CENTER_COL = COLUMNS / 2;
    constexpr Number CENTER_ROW = ROWS / 2;

    Number quadrants[2][2] = {{0, 0}, {0, 0}};

    auto robots100 = robots | std::views::transform([](const Robot& robot) {
        return teleport<COLUMNS, ROWS>(robot, 100);
    });

    auto in_quadrants = robots100 | std::views::filter([](const Point& position) {
        return position.x != CENTER_COL && position.y != CENTER_ROW;
    });

    for (auto position : in_quadrants)
    {
        quadrants[position.x > CENTER_COL][position.y > CENTER_ROW]++;
    }

    return quadrants[0][0] * quadrants[0][1] * quadrants[1][0] * quadrants[1][1];
}

TEST_CASE("Sample Single")
{
    Robot robot{{2, 4}, {2, -3}};
    SUBCASE("Simulate")
    {
        CHECK_EQ(simulate<11, 7>(robot, 0), Point{2, 4});
        CHECK_EQ(simulate<11, 7>(robot, 1), Point{4, 1});
        CHECK_EQ(simulate<11, 7>(robot, 2), Point{6, 5});
        CHECK_EQ(simulate<11, 7>(robot, 3), Point{8, 2});
        CHECK_EQ(simulate<11, 7>(robot, 4), Point{10, 6});
        CHECK_EQ(simulate<11, 7>(robot, 5), Point{1, 3});
    }
    SUBCASE("Teleport")
    {
        CHECK_EQ(teleport<11, 7>(robot, 0), Point{2, 4});
        CHECK_EQ(teleport<11, 7>(robot, 1), Point{4, 1});
        CHECK_EQ(teleport<11, 7>(robot, 2), Point{6, 5});
        CHECK_EQ(teleport<11, 7>(robot, 3), Point{8, 2});
        CHECK_EQ(teleport<11, 7>(robot, 4), Point{10, 6});
        CHECK_EQ(teleport<11, 7>(robot, 5), Point{1, 3});
    }
}

bool non_decreasing(const std::span<int>& per_row)
{
    int decreasing = 0;
    for (size_t i = 1; i < per_row.size() && decreasing < 2; ++i)
    {
        if (per_row[i] < per_row[i - 1])
        {
            return ++decreasing;
        }
    }
    return decreasing < 2;
}

template <Number COLUMNS, Number ROWS>
Number find_tree(Robots& robots)
{
    std::array<char, COLUMNS> map[ROWS];
    for (Number i = 0; i < ROWS; ++i)
    {
        std::fill(begin(map[i]), end(map[i]), '.');
    }

    auto move = [](Robot& robot) {
        robot.position.x = (COLUMNS + robot.position.x + robot.velocity.x) % COLUMNS;
        robot.position.y = (ROWS + robot.position.y + robot.velocity.y) % ROWS;
    };

    int steps = 0;
    std::array<int, ROWS> per_row;
    for (auto& robot: robots)
    {
        map[robot.position.y][robot.position.x] = '@';
        per_row[robot.position.y]++;
    }


    do
    {
        for (auto& robot : robots)
        {
            per_row[robot.position.y]--;
            map[robot.position.y][robot.position.x] = '.';
            move(robot);
            map[robot.position.y][robot.position.x] = '@';
            per_row[robot.position.y]++;
        }

        ++steps;
        if (!non_decreasing(per_row))
        {
            continue;
        }

        std::cout << "Step " << steps << std::endl;
        for (const auto& row : map)
        {
            std::cout.write(row.data(), COLUMNS);
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    while (steps < 100000);
    return steps;
}

TEST_CASE("Sample")
{
    auto robots = read_input("sample.txt");
    SUBCASE("Part 1")
    {
        CHECK(safety<11, 7>(robots) == 12);
    }
    SUBCASE("Part 2")
    {
        //CHECK(find_tree<11, 7>(robots) == 12);
    }
}

TEST_CASE("Input")
{
    auto robots = read_input("input.txt");
    SUBCASE("Part 1")
    {
        CHECK(safety<101, 103>(robots) == 226179492);
    }
    SUBCASE("Part 2")
    {
        CHECK(6 + 7 == 13);
    }
}
