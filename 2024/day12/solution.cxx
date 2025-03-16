#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

using Map = std::vector<std::string>;

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

struct Region
{
    char symbol;
    size_t area;
    size_t perimeter;
};

int64_t price_regions(const Map& map) {
    if (map.empty()) return {};

    std::vector<std::vector<bool>> visited(map.size(), std::vector<bool>(map[0].size(), false));
    
    auto flood_fill = [&](size_t row, size_t col, char symbol) -> Region {
        Region region{symbol, 0, 0};
        std::queue<std::pair<size_t, size_t>> queue;
        queue.push({row, col});
        visited[row][col] = true;
        
        while (!queue.empty())
        {
            auto [r, c] = queue.front();
            queue.pop();
            region.area++;
            
            const int dr[] = {-1, 1, 0, 0};
            const int dc[] = {0, 0, -1, 1};
            
            for (int i = 0; i < 4; i++) {
                size_t nr = r + dr[i];
                size_t nc = c + dc[i];
                
                if (map[nr][nc] != symbol)
                {
                    region.perimeter++;
                }
                else if (!visited[nr][nc])
                {
                    visited[nr][nc] = true;
                    queue.push({nr, nc});
                }
            }
        }
        return region;
    };
    
    int64_t price = 0;

    for (size_t i = 1; i < map.size() - 1; i++) {
        for (size_t j = 1; j < map[i].size() - 1; j++) {
            if (!visited[i][j]) {
                auto region = flood_fill(i, j, map[i][j]);
                price += region.area * region.perimeter;
            }
        }
    }

    return price;
}

namespace std
{
    template<>
    struct hash<std::pair<size_t, size_t>>
    {
        size_t operator()(const std::pair<size_t, size_t>& p) const
        {
            return std::hash<size_t>{}(p.first) ^ std::hash<size_t>{}(p.second);
        }
    };
}

int64_t discount_price(const Map& map) {
    if (map.empty()) return {};

    std::vector<std::vector<bool>> visited(map.size(), std::vector<bool>(map[0].size(), false));
    
    
    auto flood_fill = [&](size_t row, size_t col, char symbol) -> Region {
        Region region{symbol, 0, 0};
        std::queue<std::pair<size_t, size_t>> queue;
        queue.push({row, col});
        visited[row][col] = true;
        
        std::unordered_set<std::pair<size_t, size_t>> angle;
        
        while (!queue.empty())
        {
            auto [r, c] = queue.front();
            queue.pop();
            region.area++;
            
            {
                const int dr[] = {-1, 1, 0, 0};
                const int dc[] = {0, 0, -1, 1};
                
                for (int i = 0; i < 4; i++) {
                    size_t nr = r + dr[i];
                    size_t nc = c + dc[i];
                    
                    if (map[nr][nc] == symbol && !visited[nr][nc])
                    {
                        visited[nr][nc] = true;
                        queue.push({nr, nc});
                    }
                }
            }

            if ((symbol != map[r][c+1] && symbol != map[r+1][c]) ||
                (symbol == map[r][c+1] && symbol == map[r+1][c] && symbol != map[r+1][c+1]))
            {
                if (!angle.insert({r, c}).second)
                {
                    if (symbol == map[r+1][c+1] && symbol != map[r+1][c] && symbol != map[r][c+1])
                    {
                        ++region.perimeter;
                    }
                }
            }

            if ((symbol != map[r+1][c] && symbol != map[r][c-1]) ||
                (symbol == map[r+1][c] && symbol == map[r][c-1] && symbol != map[r+1][c-1]))
            {
                if(!angle.insert({r, c-1}).second)
                {
                    if (symbol == map[r+1][c-1] && symbol != map[r+1][c] && symbol != map[r][c-1])
                    {
                        ++region.perimeter;
                    }
                }
            }

            
            if ((symbol != map[r-1][c] && symbol != map[r][c-1]) ||
                (symbol == map[r-1][c] && symbol == map[r][c-1] && symbol != map[r-1][c-1]))
            {
                if (!angle.insert({r-1, c-1}).second)
                {
                    if (symbol == map[r-1][c-1] && symbol != map[r-1][c] && symbol != map[r][c-1])
                    {
                        ++region.perimeter;
                    }
                }
            }

            
            if ((symbol != map[r-1][c] && symbol != map[r][c+1]) ||
                (symbol == map[r-1][c] && symbol == map[r][c+1] && symbol != map[r-1][c+1]))
            {
                if (!angle.insert({r-1, c}).second)
                {
                    if (symbol == map[r-1][c+1] && symbol != map[r-1][c] && symbol != map[r][c+1])
                    {
                        ++region.perimeter;
                    }
                }
            }
        }
        
        region.perimeter += angle.size();

        return region;
    };
    
    int64_t price = 0;

    for (size_t i = 1; i < map.size() - 1; i++) {
        for (size_t j = 1; j < map[i].size() - 1; j++) {
            if (!visited[i][j]) {
                auto region = flood_fill(i, j, map[i][j]);
                price += region.area * region.perimeter;
            }
        }
    }

    return price;
}

TEST_CASE("Sample")
{
    auto map = read_input("sample.txt");
    SUBCASE("Part 1")
    {
        CHECK(price_regions(map) == 1930);

    }
    SUBCASE("Part 2")
    {
        CHECK(discount_price(map) == 1206);
    }
}

TEST_CASE("Sample2")
{
    auto map = read_input("sample2.txt");
    SUBCASE("Part 1")
    {
        CHECK(price_regions(map) == 140);

    }
    SUBCASE("Part 2")
    {
        CHECK(discount_price(map) == 80);
    }
}

TEST_CASE("Sample3")
{
    auto map = read_input("sample3.txt");
    SUBCASE("Part 1")
    {
        CHECK(price_regions(map) == 692);

    }
    SUBCASE("Part 2")
    {
        CHECK(discount_price(map) == 236);
    }
}

TEST_CASE("Sample4")
{
    auto map = read_input("sample4.txt");
    SUBCASE("Part 1")
    {
        CHECK(price_regions(map) == 1184);

    }
    SUBCASE("Part 2")
    {
        CHECK(discount_price(map) == 368);
    }
}

TEST_CASE("Input")
{
    auto map = read_input("input.txt");
    SUBCASE("Part 1")
    {
        CHECK(price_regions(map) == 1546338);
    }
    SUBCASE("Part 2")
    {
        CHECK(discount_price(map) > 974101);
        CHECK(discount_price(map) == 978590);
    }
}
