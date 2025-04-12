#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

typedef std::vector<std::string> Map;

Map read_map(const char* input_name)
{
    using namespace std;
    std::ifstream input(input_name);
    Map the_map;

    std::string line;

    the_map.push_back(line);
    the_map.push_back(line);
    while (getline(input, line))
    {
        the_map.push_back(".."s + line + ".."s);
    }

    the_map[0] = std::string(the_map.back().length(), '.');
    the_map[1] = std::string(the_map.back().length(), '.');
    line = the_map[0];
    the_map.push_back(line);
    the_map.push_back(line);

    return the_map;
}

struct Position
{
    unsigned x;
    unsigned y;
    auto operator<=>(const Position& rhs) const = default;
};

std::ostream& operator<<(std::ostream& out, const Position& p)
{
    return out << '(' << p.x << ", " << p.y << ')';
}


typedef std::array<Position, 2> Wavefront;

Position find_start(const Map& the_map)
{
    for (auto y = 0u; y < the_map.size(); ++y)
    {
        for (auto x = 0u; x < the_map[0].size(); ++x)
        {
            if (the_map[y][x] == 'S')
            {
                return Position{x, y};
            }
        }
    }
    return Position{0, 0};
}

Wavefront neighbours(const Map& the_map, Position current)
{
    switch (the_map[current.y][current.x])
    {
        case '|':
            return Wavefront{Position{current.x, current.y - 1},
                             Position{current.x, current.y + 1}};
        case '-':
            return Wavefront{Position{current.x - 1, current.y},
                             Position{current.x + 1, current.y}};
        case 'L':
            return Wavefront{Position{current.x, current.y - 1},
                             Position{current.x + 1, current.y}};
        case 'J':
            return Wavefront{Position{current.x, current.y - 1},
                             Position{current.x - 1, current.y}};
        case '7':
            return Wavefront{Position{current.x, current.y + 1},
                             Position{current.x - 1, current.y}};
        case 'F':
            return Wavefront{Position{current.x, current.y + 1},
                             Position{current.x + 1, current.y}};
    }
    return Wavefront{Position{0, 0}, Position{0, 0}};
}

bool in_wave(const Wavefront& wave, Position current)
{
    return std::find(begin(wave), end(wave), current) != end(wave);
}

Wavefront start_walk(const Map& the_map, const Position& start)
{
    Wavefront wave{};
    auto next_neighbour = 0u;
    for (int delta = -1; delta <= 1; delta += 2)
    {
        auto next_y = neighbours(the_map, Position{start.x, start.y + delta});
        if (in_wave(next_y, start))
        {
            CHECK(next_neighbour < wave.size());
            wave[next_neighbour++] = Position{start.x, start.y + delta};
        }

        auto next_x = neighbours(the_map, Position{start.x + delta, start.y});
        if (in_wave(next_x, start))
        {
            CHECK(next_neighbour < wave.size());
            wave[next_neighbour++] = Position{start.x + delta, start.y};
        }
    }
    return wave;
}

Wavefront step(const Map& the_map, const Wavefront& current, const Wavefront& previous)
{
    Wavefront next_step{};
    /*
    auto next_output = begin(next_step);
    for (auto p: current)
    {
        auto next = neighbours(the_map, p);
        std::copy_if(begin(next), end(next), next_output, [previous](auto& np) {
                return !in_wave(previous, np);
        });
        CHECK(next_output != begin(next_step));
    }
    CHECK(next_output == end(next_step));
    */
    for (auto i = 0u; i < current.size(); ++i)
    {
        auto next = neighbours(the_map, current[i]);
        std::copy_if(begin(next), end(next), begin(next_step) + i, [previous](auto& np) {
                return !in_wave(previous, np);
        });
    }
    return next_step;
}

void update_walked(const Map& the_map, const Wavefront& wave, Map& walked)
{
    for (auto& p: wave)
    {
        walked[p.y][p.x] = the_map[p.y][p.x];
    }
}

char start_shape(const Position& start, const Wavefront& first_step)
{
    int dx = first_step[0].x - first_step[1].x;
    int dy = first_step[0].y - first_step[1].y;

    switch (dx)
    {
        case -2: {
                     CHECK(dy == 0);
                     return '-';
                 }
        case -1:
                 {
                     CHECK(dy * dy == 1);
                     if (dy == 1)
                     {
                         return 'F';
                     }
                     else
                     {
                         int sx = start.x - first_step[0].x;
                         return (sx != 0)? '7' : 'L';
                     }
                 }
        case  0: {
                     CHECK(dy * dy == 4);
                     return '|';
                 }
        case  1: 
                 {
                     CHECK(dy == -1);
                     return 'J';
                 }
        case  2: {
                     CHECK(dy == 0);
                     return '-';
                 }
    }
    return 'S';
}

TEST_CASE("Start Shape")
{
    Position start{2, 2};
    {
        Wavefront first_step{Position{2, 1}, Position{1, 2}};
        CHECK(start_shape(start, first_step) == 'J');
    }
    {
        Wavefront first_step{Position{2, 1}, Position{2, 3}};
        CHECK(start_shape(start, first_step) == '|');
    }
    {
        Wavefront first_step{Position{2, 1}, Position{3, 2}};
        CHECK(start_shape(start, first_step) == 'L');
    }
    {
        Wavefront first_step{Position{1, 2}, Position{2, 3}};
        CHECK(start_shape(start, first_step) == '7');
    }
    {
        Wavefront first_step{Position{1, 2}, Position{3, 2}};
        CHECK(start_shape(start, first_step) == '-');
    }
    {
        Wavefront first_step{Position{2, 3}, Position{3, 2}};
        CHECK(start_shape(start, first_step) == 'F');
    }
}

int farthest(const Map& the_map, Map& walked)
{
    auto start_position = find_start(the_map);
    auto current_wave = start_walk(the_map, start_position);

    walked[start_position.y][start_position.x] = start_shape(start_position,
                                                             current_wave);
    update_walked(the_map, current_wave, walked);

    auto prev_wave = Wavefront{start_position, start_position};
    int steps = 1;
    while (current_wave[0] != current_wave[1])
    {
        auto next_wave = step(the_map, current_wave, prev_wave);
        prev_wave = current_wave;
        current_wave = next_wave;
        update_walked(the_map, current_wave, walked);
        ++steps;
    }
    return steps;
}

Map create_walked(const Map& the_map)
{
    std::string line(the_map[0].size(), 'i');
    auto walked = Map(the_map.size(), line);
    walked.front() = std::string(the_map[0].size(), 'o');
    walked.back() = std::string(the_map[0].size(), 'o');
    for (auto& l: walked)
    {
        l.front() = 'o';
        l.back() = 'o';
    }
    return walked;
}

// The commented out code is attempt to solve it by coloring the graph
void push_neighbours(char shape, const Position& current, const Map& walked,
                     std::queue<Position>& wave)
{
    /* auto push_if_not = [&walked, &wave](unsigned x, unsigned y, char stop) { */
    /*     if (walked[y][x] != 'o' && walked[y][x] != stop) */
    /*     { */
    /*         wave.push(Position{x, y}); */
    /*     } */
    /* }; */
    auto push_if_empty = [&walked, &wave](unsigned x, unsigned y) {
        if (walked[y][x] == 'i')
        {
            wave.push(Position{x, y});
        }
    };
    switch (shape)
    {
        case 'i':
            {
                /* push_if_not(current.x - 1, current.y - 1, 'F'); */
                /* push_if_not(current.x - 1, current.y,     '|'); */
                /* push_if_not(current.x - 1, current.y + 1, 'L'); */
                /* push_if_not(current.x    , current.y - 1, '-'); */
                /* push_if_not(current.x    , current.y + 1, '-'); */
                /* push_if_not(current.x + 1, current.y - 1, '7'); */
                /* push_if_not(current.x + 1, current.y,     '|'); */
                /* push_if_not(current.x + 1, current.y + 1, 'J'); */
                push_if_empty(current.x, current.y - 1);
                push_if_empty(current.x - 1, current.y);
                push_if_empty(current.x, current.y + 1);
                push_if_empty(current.x + 1, current.y);
                break;
            }
        case 'L':
        case '7':
        case 'J':
        case 'F':
            {
                /* wave.push(Position{current.x, current.y - 1}); */
                /* wave.push(Position{current.x - 1, current.y}); */
                /* wave.push(Position{current.x, current.y + 1}); */
                /* wave.push(Position{current.x + 1, current.y}); */
                break;
            }
        case '|':
            {
                /* wave.push(Position{current.x, current.y - 1}); */
                /* wave.push(Position{current.x, current.y + 1}); */
                break;
            }
        case '-':
            {
                /* wave.push(Position{current.x - 1, current.y}); */
                /* wave.push(Position{current.x + 1, current.y}); */
                break;
            }
    }
}

void print(const Map& the_map)
{
    std::cout << std::endl;
    for (auto& line: the_map)
    {
        std::cout << line << std::endl;
    }
    std::cout << std::endl;
}

void walk_outside(Map& walked)
{
    std::queue<Position> wave;

    wave.push(Position{1, 1});

    while (!wave.empty())
    {
        auto current = wave.front();
        wave.pop();
        auto shape = walked[current.y][current.x];
        if (shape == 'o')
        {
            continue;
        }
        push_neighbours(shape, current, walked, wave);
        /* std::cout << current << std::endl; */
        /* print(walked); */
        walked[current.y][current.x] = 'o';
    }
}

bool is_outside(const Map& walked, unsigned x, unsigned y, int dx, int dy)
{
    bool top = true;
    bool bottom = true;
    while (1)
    {
        x += dx;
        y += dy;
        switch (walked[y][x])
        {
            case 'o': return top || bottom;
            case '|':
                      {
                          bottom = !bottom;
                          top = !top;
                          break;
                      }
            case 'L':
            case 'J':
                      {
                          top = !top;
                          break;
                      }
            case 'F':
            case '7':
                      {
                          bottom = !bottom;
                          break;
                      }
        }
    }
    return top || bottom;
}

bool is_inside(const Map& walked, unsigned x, unsigned y)
{
    bool outside = true;
    outside = outside && is_outside(walked, x, y, -1, 0);
    /* outside = outside && is_outside(walked, x, y, 0, -1); */
    /* outside = outside && is_outside(walked, x, y, 1, 0); */
    /* outside = outside && is_outside(walked, x, y, 0, 1); */
    return !outside;
}

void mark_inside(Map& walked)
{
    auto rows = walked.size();
    auto cols = walked[0].size();
    for (auto y = 0u; y < rows; ++y)
    {
        for (auto x = 0u; x < cols; ++x)
        {
            if (walked[y][x] != 'i')
            {
                continue;
            }
            if (!is_inside(walked, x, y))
            {
                walked[y][x] = 'o';
            }
        }
    }
}

auto count_inside(const Map& walked)
{
    auto count = 0ull;
    for (auto& line: walked)
    {
        count += std::count(begin(line), end(line), 'i');
    }
    return count;
}

TEST_CASE("Sample 1")
{
    auto the_map = read_map("sample.txt");
    auto walked = the_map;
    SUBCASE("Part 1")
    {
        auto start = find_start(the_map);
        CHECK(start == Position{2, 4});

        CHECK(farthest(the_map, walked) == 8);
    }
}

TEST_CASE("Sample 2")
{
    auto the_map = read_map("sample2.txt");
    auto walked = create_walked(the_map);
    SUBCASE("Part 2")
    {
        farthest(the_map, walked);
        walk_outside(walked);
        mark_inside(walked);
        auto r = count_inside(walked);
        CHECK(r == 8);
    }
}

TEST_CASE("Sample 3")
{
    auto the_map = read_map("sample3.txt");
    auto walked = create_walked(the_map);
    SUBCASE("Part 2")
    {
        farthest(the_map, walked);
        walk_outside(walked);
        mark_inside(walked);
        auto r = count_inside(walked);
        CHECK(r == 10);
    }
}

TEST_CASE("Sample 4")
{
    auto the_map = read_map("sample4.txt");
    auto walked = create_walked(the_map);
    SUBCASE("Part 2")
    {
        farthest(the_map, walked);
        walk_outside(walked);
        mark_inside(walked);
        auto r = count_inside(walked);
        CHECK(r == 4);
    }
}


TEST_CASE("Input")
{
    auto the_map = read_map("input.txt");
    auto walked = create_walked(the_map);
    SUBCASE("Part 1")
    {
        auto r = farthest(the_map, walked);
        std::cout << "part 1: " << r << std::endl;
        CHECK(r == 6828);
    }
    SUBCASE("Part 2")
    {
        farthest(the_map, walked);
        walk_outside(walked);
        mark_inside(walked);
        auto r = count_inside(walked);
        std::cout << "part 2: " << r << std::endl;
        CHECK(r == 459);
    }
}
