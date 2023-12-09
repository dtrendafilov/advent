#include <cstdint>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string_view>
#include <unordered_map>
#include <regex>
#include <string>
#include <future>
#include <numeric>


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

#include "../darllen.hxx"


typedef std::string NodeName;
typedef std::tuple<NodeName, NodeName> Neighbours;
typedef std::unordered_map<NodeName, Neighbours> Map;


struct Instruction
{
    std::string _instructions;
    size_t _current = 0;
    char next()
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


int navigate(const Map& map, Instruction& instruction)
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

struct Loop
{
    uint64_t position;
    uint64_t steps;
    std::strong_ordering operator <=>(const Loop& rhs) const = default;
};

std::ostream& operator<<(std::ostream& out, const Loop& l)
{
    return out << "Loop{" << l.position << ", " << l.steps << '}';
}

typedef std::vector<Loop> Loops;

Loop loop(const Map& map, Instruction instruction, NodeName start)
{
    std::unordered_map<NodeName, Loops> final_states;
    auto current = map.find(start);
    CHECK(current != map.end());
    uint64_t steps = 0;

    auto do_step = [&]() {
        auto& next = (instruction.next() == 'L')? get<0>(current->second)
                                                : get<1>(current->second);
        current = map.find(next);
        ++steps;
    };

    while (1)
    {
        while (current != map.end() && current->first.back() != 'Z')
        {
            do_step();
        }
        CHECK(current != map.end());

        auto& finals = final_states[current->first];
        Loop potential_loop{instruction._current, steps};
        auto i = std::lower_bound(begin(finals), end(finals), potential_loop);
        if (i == end(finals))
        {
            if (!finals.empty() 
                    && finals.back().position == potential_loop.position)
            {
                auto& start = finals.back();
                CHECK(potential_loop.steps > start.steps);
                std::cerr << potential_loop << ' ' << start << std::endl;
                return Loop{potential_loop.steps - start.steps, start.steps};
            }
            finals.push_back(potential_loop);
        }
        else if (i->position == potential_loop.position)
        {
            CHECK(potential_loop.steps > i->steps);
            std::cerr << potential_loop << ' ' << *i << std::endl;
            return Loop{potential_loop.steps - i->steps, i->steps};
        }
        else
        {
            finals.insert(i, potential_loop);
        }

        do_step();
    }
    CHECK(false);
    return Loop{};
}

int navigate_ghost(const Map& map, Instruction& instruction)
{
    auto locations = start_locations(map);
    CHECK(!locations.empty());
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
        ++steps;
    }
    return steps;
}

int navigate_ghost_lcm(const Map& map, Instruction& instruction)
{
    auto locations = start_locations(map);
    CHECK(!locations.empty());
    std::vector<std::future<Loop>> loops(locations.size());
    std::transform(begin(locations), end(locations), begin(loops),
            [&map, instruction](auto location) {
                return std::async(loop, map, instruction, location->first);
    });
    Loop l{1, 1};
    for (auto& future_r: loops)
    {
            auto r = future_r.get();
            auto loop_step = l.position;
            auto steps = l.steps;
            while ((steps - r.steps) % r.position != 0)
            {
                std::cerr 
                    << "l(" << l.position << ',' << l.steps << ')' << ' '
                    << "r(" << r.position << ',' << r.steps << ')' << ' '
                    << steps << ':'
                    << ((steps - l.steps) % l.position) << ' '
                    << ((steps - r.steps) % r.position) << std::endl;
                steps += loop_step;
            }
            l = Loop{std::lcm(l.position, r.position), steps};
    }
    return l.steps;
}

TEST_CASE("Loop")
{
    auto [instructions, map] = read_map("sample3.txt");
    auto loop1 = loop(map, instructions, "PPA");
    CHECK(loop1 == Loop{2, 2});
    auto loop2 = loop(map, instructions, "QQA");
    CHECK(loop2 == Loop{3, 3});
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
        auto [instructions, map] = read_map("sample3.txt");
        CHECK(navigate_ghost(map, instructions) == 6);
    }
    SUBCASE("Part 2 - LCM")
    {
        /* auto [instructions, map] = read_map("sample3.txt"); */
        /* CHECK(navigate_ghost_lcm(map, instructions) == 6); */
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
        /* auto [instructions, map] = read_map("input.txt"); */
        /* auto steps = navigate_ghost(map, instructions); */
        /* std::cout << "part2: " << steps << std::endl; */
    }
}
