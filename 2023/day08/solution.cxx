#include <iostream>
#include <fstream>
#include <algorithm>
#include <string_view>
#include <unordered_map>
#include <regex>
#include <string>


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

#include "../darllen.hxx"


typedef std::string NodeName;
typedef std::tuple<NodeName, NodeName> Neighbours;
typedef std::unordered_map<NodeName, Neighbours> Map;


struct Instruction
{
    std::string _instructions;
    mutable size_t _current = 0;
    char next() const
    {
        auto n = _instructions[_current];
        _current = (_current + 1) % _instructions.size();
        return n;
    }
};

std::tuple<Instruction, Map> read_map(const char* input_name)
{
    std::fstream input(input_name);
    std::string instructions;

    getline(input, instructions);

    std::string line;
    getline(input, line); // skip empty line

    Map map;
    std::regex split_re("([[:upper:]]{3}) = \\(([[:upper:]]{3}), ([[:upper:]]{3})\\)",
                        std::regex::extended);
    while (getline(input, line))
    {
        NodeName name;
        Neighbours neighbours;
        if (!std::regex_search(line, split_re))
        {
            std::cerr << '|' << line << '|' << std::endl;
            CHECK(false);
        }


        if (darllen::read(split_re, begin(line), end(line), name,
                    get<0>(neighbours),
                    get<1>(neighbours)))
        {
            map[name] = std::move(neighbours);
        }
        else
        {
            CHECK(false);
        }
    }
    return std::make_tuple(Instruction{instructions}, std::move(map));
}


int navigate(const Map& map, const Instruction& instruction)
{
    auto current = map.find("AAA");
    CHECK(current != map.end());
    int steps = 0;
    while (current != map.end() && current->first != "ZZZ")
    {
        auto& next = (instruction.next() == 'L')? get<0>(current->second)
                                                : get<1>(current->second);
        current = map.find(next);
        ++steps;
    }
    CHECK(current != map.end());
    //CHECK(current->first == "ZZZ");
    return steps;
}

typedef std::vector<Map::const_iterator> Locations;
Locations start_locations(const Map& map)
{
    Locations result;
    for (auto l = begin(map); l != end(map); ++l)
    {
        if (l->first.back() == 'A')
        {
            result.push_back(l);
        }
    }
    return result;
}

bool is_final(const Locations& locations)
{
    return std::all_of(begin(locations), end(locations), [](auto l) {
            return l->first.back() == 'Z';
    });
}

int navigate_ghost(const Map& map, const Instruction& instruction)
{
    auto locations = start_locations(map);
    CHECK(!locations.empty());
    locations.erase(begin(locations), begin(locations) + 3);
    int steps = 0;
    while (!is_final(locations))
    {
        const auto next = instruction.next();
        for (auto& current : locations)
        {
            const auto& next_location = (next == 'L')? get<0>(current->second)
                                           : get<1>(current->second);
            current = map.find(next_location);
            CHECK(current != map.end());
        }
        if (steps % 10000 == 0)
        {
            std::cerr << steps << std::endl;
        }
        ++steps;
    }
    return steps;
}


TEST_CASE("Sample 1")
{
    SUBCASE("Part 1")
    {
        {
            auto [instructions, map] = read_map("sample1.txt");
            CHECK(navigate(map, instructions) == 2);
        }
        {
            auto [instructions, map] = read_map("sample2.txt");
            CHECK(navigate(map, instructions) == 6);
        }
    }

    SUBCASE("Part 2")
    {
        /* auto [instructions, map] = read_map("sample3.txt"); */
        /* CHECK(navigate_ghost(map, instructions) == 6); */
    }
}

TEST_CASE("Input")
{
    SUBCASE("Part 1")
    {
        auto [instructions, map] = read_map("input.txt");
        auto steps = navigate(map, instructions);
        CHECK(steps == 12169);
        std::cout << "part1: " << steps << std::endl;
    }
    SUBCASE("Part 2")
    {
        auto [instructions, map] = read_map("input.txt");
        auto steps = navigate_ghost(map, instructions);
        std::cout << "part2: " << steps << std::endl;
    }
}
