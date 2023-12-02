#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <string_view>
#include <ranges>
#include <cassert>
#include <numeric>

#include "../darllen.hxx"

struct Bag
{
    int red = 0;
    int green = 0;
    int blue = 0;
};
typedef std::vector<Bag> Bags;

struct Game
{
    int id = 0;
    Bags draws;
};
typedef std::vector<Game> Games;

#if __cpp_lib_generator >= 202207L
#include <generator>
std::generator<std::string> lines(const char* input_name)
{
    std::fstream input(input_name);
    std::string line;
    while (std::getline(input, line)) {
        co_yield line;
    }
}
#endif

typedef std::vector<std::string> Lines;
Lines read_lines(const char* input_name)
{
    std::fstream input(input_name);
    std::string line;
    Lines lines;
    while (std::getline(input, line)) {
        lines.emplace_back(line);
    }
    return lines;
}

struct GameLoader
{
    std::regex game_id_rx{"Game ([[:digit:]]+):"};
    std::regex red_rx{"([[:digit:]]+) red"};
    std::regex green_rx{"([[:digit:]]+) green"};
    std::regex blue_rx{"([[:digit:]]+) blue"};

    Bag read_bag(std::string_view text) const
    {
        Bag draw;
        darllen::search_read(red_rx, begin(text), end(text), draw.red);
        darllen::search_read(green_rx, begin(text), end(text), draw.green);
        darllen::search_read(blue_rx, begin(text), end(text), draw.blue);
        /* std::cerr << draw.red << ' ' << draw.green << ' ' << draw.blue << std::endl; */
        return draw;
    }

    Game operator()(std::string_view line) const
    {
        Game game;
        /* std::cerr << "parsing game from " << line << std::endl; */
        if (darllen::search_read(game_id_rx, begin(line), end(line), game.id))
        {
            auto draw_start = line.find(':');
            assert(draw_start != std::string_view::npos);

            std::istringstream draws_input(std::string(line.substr(draw_start + 1)));
            std::string draw_text;
            /* std::cerr << game.id << std::endl; */
            while (std::getline(draws_input, draw_text, ';'))
            {
                game.draws.push_back(read_bag(draw_text));
            }
        }
        else
        {
            std::cerr << "could not read game from: " << line << std::endl;
        }
        return game;
    }
};

bool is_possible_draw(const Bag& bag, const Bag& draw)
{
    return draw.red <= bag.red 
        && draw.blue <= bag.blue
        && draw.green <= bag.green;
}

bool is_possible_game(const Bag& bag, const Game& game)
{
    return std::all_of(begin(game.draws), end(game.draws), [&bag](auto& draw) {
            return is_possible_draw(bag, draw);
    });
}

int sum_possible_games(const Bag& max, const auto& games)
{
    GameLoader loader;
#if 0
    // Each filter operation causes the transform to be invoked once more!!!
    auto possible_games = games | std::views::transform(loader)
        | std::views::filter([&max](const Game& game) {
                return is_possible_game(max, game);
                })
        | std::views::filter([&max](const Game& game) {
                return is_possible_game(max, game);
                })
        | std::views::transform([](const Game& game) { return game.id; })
    ;
#endif 
    auto possible_games = games | std::views::filter([&max](const Game& game) {
                return is_possible_game(max, game);
                })
        | std::views::transform([](const Game& game) { return game.id; })
    ;
    int sum = std::accumulate(std::ranges::begin(possible_games),
                              std::ranges::end(possible_games), 0);
    return sum;
}


int power(const Game& game)
{
    int min_red = 0;
    int min_green = 0;
    int min_blue = 0;
    for (auto& draw: game.draws)
    {
        min_red = std::max(min_red, draw.red);
        min_green = std::max(min_green, draw.green);
        min_blue = std::max(min_blue, draw.blue);
    }
    return min_red * min_green * min_blue;
}

int minimum_cubes(const Games& games)
{
    auto powers = games | std::views::transform(power);
    return std::accumulate(std::ranges::begin(powers),
                           std::ranges::end(powers), 0);
}


int main(int argc, const char* argv[])
{
    using namespace std;
    if (argc != 3) {
        cerr << "Usage: solution part input" << endl;
        return 1;
    }
    char part = *argv[1];
    auto input = argv[2];

    cout << input << endl;

    auto lines = read_lines(input);
    GameLoader loader;
    Games games{lines.size()};
    std::transform(begin(lines), end(lines), begin(games), loader);

    if (part == '1')
    {
        cout << sum_possible_games(Bag{12, 13, 14}, games) << endl;
    }
    else if (part == '2')
    {
        cout << minimum_cubes(games) << endl;
    }
    else
    {
        cerr << "Unknown part: " << part << endl;
    }



    return 0;
}
