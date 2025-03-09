#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

constexpr size_t MAX_SCORE = 240;

using Map = std::vector<std::string>;
using Score = std::optional<std::bitset<MAX_SCORE>>;
using Scores = std::vector<std::vector<Score>>;


constexpr int NOT_VISITED = -1;
using Ratings = std::vector<std::vector<int>>;

auto read_input(const char* name)
{
    std::ifstream input(name);
    Map map;
    std::string line;

    if (std::getline(input, line)) {
        map.push_back(std::string(line.size() + 2, '#'));
        map.push_back('#' + line + '#');
    }
    
    while (std::getline(input, line))
    {
        map.push_back('#' + line + '#');
    }
    if (!map.empty()) {
        map.push_back(std::string(map[0].size(), '#'));
    }
    return map;
}

auto score(const Map& map, int i, int j, Scores& scores)
{
    if (scores[i][j].has_value())
    {
        return scores[i][j].value();
    }

    if (map[i][j] == '#')
    {
        scores[i][j] = std::bitset<MAX_SCORE>{};
        return scores[i][j].value();
    }

    std::bitset<MAX_SCORE> result;
    auto next = map[i][j] + 1;
    if (map[i-1][j] == next) {
        result |= score(map, i-1, j, scores);
    }
    if (map[i+1][j] == next) {
        result |= score(map, i+1, j, scores);
    }
    if (map[i][j-1] == next) {
        result |= score(map, i, j-1, scores);
    }
    if (map[i][j+1] == next) {
        result |= score(map, i, j+1, scores);
    }

    scores[i][j] = result;
    return scores[i][j].value();
}

auto compute_scores(const Map& map)
{
    Scores scores(map.size(), std::vector<Score>(map[0].size(), Score{}));

    size_t peaks = 0;
    for (size_t i = 0; i < map.size(); ++i)
    {
        for (size_t j = 0; j < map[i].size(); ++j)
        {
            if (map[i][j] == '9')
            {
                auto score = std::bitset<MAX_SCORE>{};
                CHECK(peaks < MAX_SCORE);
                score.set(peaks);
                scores[i][j] = score;
                ++peaks;
            }
        }
    }

    for (size_t i = 0; i < map.size(); ++i)
    {
        for (size_t j = 0; j < map[i].size(); ++j)
        {
            score(map, i, j, scores);
        }
    }

    return scores;
}

auto rate(const Map& map, int i, int j, Ratings& ratings)
{
    if (ratings[i][j] != NOT_VISITED)
    {
        return ratings[i][j];
    }

    auto result = 0;
    auto next = map[i][j] + 1;
    if (map[i-1][j] == next) {
        result += rate(map, i-1, j, ratings);
    }
    if (map[i+1][j] == next) {
        result += rate(map, i+1, j, ratings);
    }
    if (map[i][j-1] == next) {
        result += rate(map, i, j-1, ratings);
    }
    if (map[i][j+1] == next) {
        result += rate(map, i, j+1, ratings);
    }

    ratings[i][j] = result;
    return ratings[i][j];
}

auto compute_ratings(const Map& map)
{
    Ratings ratings(map.size(), std::vector<int>(map[0].size(), NOT_VISITED));

    for (size_t i = 0; i < map.size(); ++i)
    {
        for (size_t j = 0; j < map[i].size(); ++j)
        {
            if (map[i][j] == '9')
            {
                ratings[i][j] = 1;
            }
            else if (map[i][j] == '#')
            {
                ratings[i][j] = 0;
            }
        }
    }

    for (size_t i = 0; i < map.size(); ++i)
    {
        for (size_t j = 0; j < map[i].size(); ++j)
        {
            rate(map, i, j, ratings);
        }
    }

    return ratings;
}



int score_trailheads(const Map& map, const Scores& scores)
{
    int result = 0;
    for (size_t i = 0; i < map.size(); ++i)
    {
        for (size_t j = 0; j < map[i].size(); ++j)
        {
            if (map[i][j] == '0')
            {
                result += scores[i][j]->count();
            }
        }
    }
    return result;
}

int rate_trailheads(const Map& map, const Ratings& ratings)
{
    int result = 0;
    for (size_t i = 0; i < map.size(); ++i)
    {
        for (size_t j = 0; j < map[i].size(); ++j)
        {
            if (map[i][j] == '0')
            {
                result += ratings[i][j];
            }
        }
    }
    return result;
}


TEST_CASE("Sample")
{
    auto map = read_input("sample.txt");
    
    SUBCASE("Part 1")
    {
        auto scores = compute_scores(map);
        CHECK(score_trailheads(map, scores) == 36);
    }
    SUBCASE("Part 2")
    {
        auto ratings = compute_ratings(map);
        CHECK(rate_trailheads(map, ratings) == 81);
    }
}

TEST_CASE("Input")
{
    auto map = read_input("input.txt");
    SUBCASE("Part 1")
    {
        auto scores = compute_scores(map);
        CHECK(score_trailheads(map, scores) == 659);
    }
    SUBCASE("Part 2")
    {
        auto ratings = compute_ratings(map);
        CHECK(rate_trailheads(map, ratings) == 1463);
    }
}
