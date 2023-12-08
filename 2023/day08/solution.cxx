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
#include <limits>


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
    uint64_t length;
    std::strong_ordering operator <=>(const Loop& rhs) const = default;
};

std::ostream& operator<<(std::ostream& out, const Loop& l)
{
    return out << "Loop{" << l.position << ", "
                          << l.steps << ", "
                          << l.length << '}';
}

typedef std::vector<Loop> Loops;

Loops find_loops(const Map& map, Instruction instruction, NodeName start)
{
    std::unordered_map<NodeName, Loops> final_states;
    auto current = map.find(start);
    CHECK(current != map.end());
    uint64_t steps = 0;

    Loops loops;


    auto do_step = [&]() {
        auto& next = (instruction.next() == 'L')? get<0>(current->second)
                                                : get<1>(current->second);
        current = map.find(next);
        ++steps;
    };

    auto limit = std::numeric_limits<uint64_t>::max();
    while (steps < limit)
    {
        while (steps < limit && current != map.end()
                && current->first.back() != 'Z')
        {
            do_step();
        }
        CHECK(current != map.end());
        if (steps >= limit)
        {
            break;
        }

        auto& finals = final_states[current->first];
        Loop potential_loop{instruction._current, steps, 0};
        auto i = std::lower_bound(begin(finals), end(finals), potential_loop);
        if (i == end(finals))
        {
            if (!finals.empty() 
                    && finals.back().position == potential_loop.position)
            {
                auto& start = finals.back();
                CHECK(potential_loop.steps > start.steps);
                loops.emplace_back(Loop{start.position,
                                        start.steps,
                                        potential_loop.steps - start.steps});
                if (limit == std::numeric_limits<uint64_t>::max())
                {
                    limit = steps + loops.back().length;
                }
            }
            else
            {
                finals.push_back(potential_loop);
            }
        }
        else if (i != begin(finals) && (--i)->position == potential_loop.position)
        {
            CHECK(potential_loop.steps > i->steps);
            loops.emplace_back(Loop{i->position,
                                    i->steps,
                                    potential_loop.steps - i->steps});
            if (limit == std::numeric_limits<uint64_t>::max())
            {
                limit = steps + loops.back().length;
            }
        }
        else
        {
            finals.insert(i, potential_loop);
        }

        do_step();
    }
    std::sort(begin(loops), end(loops));
    return loops;
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

auto possible_positions(const Loops& loops)
{
    std::vector<uint64_t> positions(loops.size());
    std::transform(begin(loops), end(loops), begin(positions),
            [](const Loop& loop) {
                return loop.position;
    });
    return positions;
}

template <typename It, typename Map, typename Reduce>
auto map_reduce(It begin, It end, const Map& map, const Reduce& reduce)
{
    auto result = map(*begin++);
    while (begin != end)
    {
        result = reduce(result, map(*begin++));
    }
    return result;
}

template <typename It, typename Reduce>
auto my_reduce(It begin, It end, Reduce reduce)
{
    auto result = *begin++;
    while (begin != end)
    {
        result = reduce(result, *begin++);
    }
    return result;
}

Loops get_loops_at_pos(const std::vector<Loops>& all_loops, uint64_t position)
{
    Loops result(all_loops.size());
    std::transform(begin(all_loops), end(all_loops), begin(result),
        [position](const Loops& loops)
        {
            auto p = std::lower_bound(begin(loops), end(loops), Loop{position, 0, 0});
            CHECK(p != end(loops));
            CHECK(p->position == position);
            return *p;
    });
    return result;
}

uint64_t converge_steps(const Loops& loops)
{
    return my_reduce(begin(loops), end(loops), [](const Loop& l, const Loop& r)
    {
        auto loop_step = l.length;
        auto steps = l.steps;
        while ((steps < r.steps) || (steps - r.steps) % r.length != 0)
        {
            steps += loop_step;
        }
        return Loop{r.position, steps, std::lcm(l.length, r.length)};
    }).steps;
}

uint64_t navigate_ghost_lcm(const Map& map, Instruction& instruction)
{
    auto locations = start_locations(map);
    CHECK(!locations.empty());
    std::vector<std::future<Loops>> future_loops(locations.size());
    std::transform(begin(locations), end(locations), begin(future_loops),
            [&map, instruction](auto location) {
                return std::async(find_loops, map, instruction, location->first);
    });
    std::vector<Loops> all_loops(future_loops.size());
    std::transform(begin(future_loops), end(future_loops), begin(all_loops),
            [](std::future<Loops>& future_loops) {
                return future_loops.get();
    });

    auto positions = map_reduce(begin(all_loops), end(all_loops),
            possible_positions, 
            [](const auto& l, const auto& r) {
                auto result = l;
                auto new_end = std::set_intersection(begin(l), end(l),
                                                     begin(r), end(r),
                                                     begin(result));
                result.erase(new_end, result.end());
                return result;
    });

    std::vector<uint64_t> steps(positions.size());
    std::transform(begin(positions), end(positions), begin(steps),
            [all_loops](uint64_t position) {
                auto loops_at_pos = get_loops_at_pos(all_loops, position);
                return converge_steps(loops_at_pos);
    });

    return *std::min_element(begin(steps), end(steps));
}

TEST_CASE("Loop")
{
    auto [instructions, map] = read_map("sample3.txt");
    auto loop1 = find_loops(map, instructions, "PPA");
    CHECK(loop1.size() == 1);
    CHECK(loop1[0] == Loop{0, 2, 2});
    auto loop2 = find_loops(map, instructions, "QQA");
    CHECK(loop2.size() == 2);
    CHECK(loop2[0] == Loop{0, 6, 6});
    CHECK(loop2[1] == Loop{1, 3, 6});
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
        auto [instructions, map] = read_map("sample3.txt");
        CHECK(navigate_ghost_lcm(map, instructions) == 6);
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
        auto steps = navigate_ghost_lcm(map, instructions);
        std::cout << "part2: " << steps << std::endl;
    }
}
