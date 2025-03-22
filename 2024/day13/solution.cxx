#include "precompiled.hxx"

#include "../darllen.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

using Number = int64_t;

constexpr Number COST_A = 3;
constexpr Number COST_B = 1;

struct Point
{
    Number x;
    Number y;
};

struct Game
{
    Point a;
    Point b;
    Point prize;
};

std::vector<Game> read_games(const char* filename) {
    std::vector<Game> games;
    std::ifstream file(filename);
    std::string line;
    std::string block;
    
    // Regex for "Button A: X+94, Y+34" format
    std::regex pattern(R"(Button A: X\+(\d+), Y\+(\d+)\s*Button B: X\+(\d+), Y\+(\d+)\s*Prize: X=(\d+), Y=(\d+))");
    
    // Read 3 lines at a time
    while (std::getline(file, line)) {
        block = line;
        for (int i = 0; i < 2; i++) {
            std::getline(file, line);
            block += '\n' + line;
        }
        std::getline(file, line); // skip empty line
        
        Game game;
        if (darllen::read(pattern, block.begin(), block.end(),
                         game.a.x, game.a.y,
                         game.b.x, game.b.y,
                         game.prize.x, game.prize.y)) {
            games.push_back(game);
        }
    }
    return games;
}


Number play_optimal_game(const Game& game)
{
    const auto& [a, b, p] = game;
    // Check if the prize is reachable - on the line defined by the buttons
    {
        auto t = p.x * a.y - p.y * a.x;
        auto s = b.x * a.y - b.y * a.x;
        if (t % s != 0) return 0;
    }

    auto max_b = p.x / b.x;
    auto min_cost = std::numeric_limits<Number>::max();
    for (auto nb = 0; nb <= max_b; nb++)
    {
        auto na = (p.x - nb * b.x) / a.x;
        if (na * a.y + nb * b.y == p.y)
        {
            min_cost = std::min(min_cost, na * COST_A + nb * COST_B);
        }
    }
    return min_cost != std::numeric_limits<Number>::max() ? min_cost : 0;
}


Number play_game(const Game& game)
{
    const auto& [a, b, p] = game;
    // Create a system of equations:
    // na * ax + nb * bx = px  (for x coordinate)
    // na * ay + nb * by = py  (for y coordinate)
    // where na, nb are the number of times to press buttons A and B
    
    // Matrix form:
    // [ax bx] [na] = [px]
    // [ay by] [nb]   [py]
    
    Number det = a.x * b.y - b.x * a.y;
    if (det == 0)
    {
        // Check for multiple solutions
    }
    
    // Solve using Cramer's rule
    Number na = (p.x * b.y - b.x * p.y) / det;
    Number nb = (a.x * p.y - p.x * a.y) / det;
    

    if ((na < 0 || nb < 0) ||
        (na * a.x + nb * b.x != p.x) ||
        (na * a.y + nb * b.y != p.y))
    {
        return 0;
    }

    return na * COST_A + nb * COST_B;
}

Number total_cost(const auto& games)
{
    auto do_play = std::ranges::transform_view(games, play_game);
    return std::accumulate(do_play.begin(), do_play.end(), Number{0});
}

Game fixup_prizes(const Game& game)
{
    return Game{game.a, game.b, Point{game.prize.x + 10000000000000, game.prize.y + 10000000000000}};
}

TEST_CASE("Game Solutions") {
    auto games = read_games("sample.txt");
    
    auto cost = play_game(games[0]);
    CHECK(cost == 280);
    
    cost = play_game(games[1]);
    CHECK(cost == 0);
}

TEST_CASE("Sample")
{
    auto games = read_games("sample.txt");
    SUBCASE("Part 1")
    {
        CHECK(total_cost(games) == 480);

    }
    SUBCASE("Part 2")
    {
        auto fixed_games = std::ranges::transform_view(games, fixup_prizes);
        CHECK(total_cost(fixed_games) == 875318608908);
    }
}

TEST_CASE("Input")
{
    auto games = read_games("input.txt");
    SUBCASE("Part 1")
    {
        CHECK(total_cost(games) == 40369);
    }
    SUBCASE("Part 2")
    {
        auto fixed_games = std::ranges::transform_view(games, fixup_prizes);
        CHECK(total_cost(fixed_games) == 72587986598368);
    }
}

TEST_CASE("Read Games") {
    auto games = read_games("sample.txt");
    REQUIRE(games.size() == 4);
    
    // Test first game coordinates
    CHECK(games[0].a.x == 94);
    CHECK(games[0].a.y == 34);
    CHECK(games[0].b.x == 22);
    CHECK(games[0].b.y == 67);
    CHECK(games[0].prize.x == 8400);
    CHECK(games[0].prize.y == 5400);
}
