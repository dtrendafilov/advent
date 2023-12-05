#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>
#include <numeric>
#include <ranges>
#include <vector>
#include <string>
#include <regex>
#include <array>

#include "../darllen.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

TEST_CASE("Answer is sane")
{
    CHECK(6 * 7 == 42);
}


struct Interval
{
    uint64_t source;
    uint64_t destination;
    uint64_t length;

    bool operator<(const Interval& rhs) const
    {
        return source < rhs.source;
    }
};

typedef std::vector<Interval> Map;

enum MapIndices
{
    seed_to_soil,
    soil_to_fertilizer,
    fertilizer_to_water,
    water_to_light,
    light_to_temperature,
    temperature_to_humidity,
    humidity_to_location,
    size,
};

struct Almanac
{
    std::vector<uint64_t> seeds;
    std::array<Map, MapIndices::size> maps;
};



uint64_t map_through(const Map& the_map, uint64_t number)
{
    Interval to_map{number, number, 1};
    auto bound = std::upper_bound(begin(the_map), end(the_map), to_map);

    if (bound == begin(the_map))
    {
        return number;
    }

    --bound;
    if (bound->source <= number
            && number < (bound->source + bound->length))
    {
        return bound->destination + (number - bound->source);
    }

    return number;
}

TEST_CASE("Mapping")
{
    Map the_map;
    
    the_map.emplace_back(Interval{42, 24, 8});
    the_map.emplace_back(Interval{64, 46, 6});

    SUBCASE("Map before")
    {
        CHECK(map_through(the_map, 20) == 20);
        CHECK(map_through(the_map, 41) == 41);
    }
    SUBCASE("Map inside")
    {
        CHECK(map_through(the_map, 42) == 24);
        CHECK(map_through(the_map, 45) == 27);
        CHECK(map_through(the_map, 49) == 31);

        CHECK(map_through(the_map, 64) == 46);
        CHECK(map_through(the_map, 66) == 48);
        CHECK(map_through(the_map, 69) == 51);
    }
    SUBCASE("Map after")
    {
        CHECK(map_through(the_map, 50) == 50);
        CHECK(map_through(the_map, 90) == 90);
    }
}

Almanac read(const char* input_name)
{
    Almanac almanac;

    std::ifstream input(input_name);
    std::string skip;

    input >> skip;
    CHECK(skip == "seeds:");
    uint64_t number;
    while (input >> number)
    {
        almanac.seeds.push_back(number);
    }

    for (auto& map : almanac.maps)
    {
        input.clear();
        input >> skip;
        input >> skip;
        CHECK(skip == "map:");
        uint64_t destination, source, length;
        while (input >> destination >> source >> length)
        {
            map.push_back(Interval{source, destination, length});
        }
        std::sort(begin(map), end(map));
        // check for overlap
        for (auto i = 0u; i < map.size() - 1; ++i)
        {
            CHECK(map[i].source + map[i].length <= map[i + 1].source);
        }
    }

    return almanac;
}

uint64_t map_through(const Almanac& almanac, uint64_t seed)
{
    for (auto& map: almanac.maps)
    {
        seed = map_through(map, seed);
    }
    return seed;
}


TEST_CASE("Sample")
{
    auto almanac = read("sample.txt");

    CHECK(almanac.seeds == std::vector<uint64_t>{79, 14, 55, 13});

    CHECK(almanac.maps[MapIndices::seed_to_soil].size() == 2);

    SUBCASE("Part 1")
    {
        auto seed = begin(almanac.seeds);
        auto min_location = map_through(almanac, *seed);
        while (++seed != end(almanac.seeds))
        {
            auto location = map_through(almanac, *seed);
            min_location = std::min(min_location, location);
        }
        CHECK(min_location == 35);
    }

    SUBCASE("Part 2")
    {
        auto min_location = std::numeric_limits<uint64_t>::max();
        for (auto i = 0u; i < almanac.seeds.size(); i += 2)
        {
            auto limit = almanac.seeds[i] + almanac.seeds[i+1];
            for (auto seed = almanac.seeds[i]; seed < limit; ++seed)
            {
                auto location = map_through(almanac, seed);
                min_location = std::min(min_location, location);
            }
        }
        CHECK(min_location == 46);
    }
}

TEST_CASE("Input")
{
    auto almanac = read("input.txt");

    SUBCASE("Part 1")
    {
        auto seed = begin(almanac.seeds);
        auto min_location = map_through(almanac, *seed);
        while (++seed != end(almanac.seeds))
        {
            auto location = map_through(almanac, *seed);
            min_location = std::min(min_location, location);
        }
        std::cout << "Part 1 minimal location: " << min_location << std::endl;
    }
    SUBCASE("Part 2")
    {
        auto min_location = std::numeric_limits<uint64_t>::max();
        for (auto i = 0u; i < almanac.seeds.size(); i += 2)
        {
            auto limit = almanac.seeds[i] + almanac.seeds[i+1];
            for (auto seed = almanac.seeds[i]; seed < limit; ++seed)
            {
                auto location = map_through(almanac, seed);
                min_location = std::min(min_location, location);
            }
        }
        std::cout << "Part 2 minimal location: " << min_location << std::endl;
    }
}
