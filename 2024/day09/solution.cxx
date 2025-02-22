#include "precompiled.hxx"
#include <algorithm>
#include <numeric>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

using Number = int64_t;

using Disk = std::vector<int>;

Disk from_line(const std::string_view line)
{
    Disk result(line.size());
    std::transform(begin(line), end(line), begin(result), [](char d) {
            return d - '0';
    });
    return result;
}

Disk read_input(const char* name)
{
    std::ifstream input(name);
    std::string line;
    std::getline(input, line);
    return from_line(line);
}

Number sum_pos(int pos, int count)
{
    return (Number{2} * pos + count - 1) * count / 2;
}

Number defrag_checksum(Disk& d)
{
    Number sum = 0;
    CHECK(d.size() % 2 == 1);
    int pos = 0;
    for (int front = 0, file = d.size() - 1; front <= file; ++front)
    {
        sum += sum_pos(pos, d[front]) * front / 2;
        pos += d[front];
        ++front;
        int free = d[front];
        while (free && front <= file) {
            int moved = std::min(free, d[file]);
            free -= moved;
            d[file] -= moved;
            sum += sum_pos(pos, moved) * file / 2;
            pos += moved;
            if (!d[file]) {
                file -= 2;
            }
        }
    }
    return sum;
}

Disk get_positions(const Disk& d)
{
    Disk pos(d.size() + 1);
    pos[0] = 0;
    std::partial_sum(begin(d), end(d), begin(pos) + 1);
    return pos;
}

Number defrag_by_file(Disk& d)
{
    Number sum = 0;
    CHECK(d.size() % 2 == 1);
    Disk pos = get_positions(d);
    for (int file = d.size() - 1; file >= 0; file -= 2)
    {
        int free = 1;
        while (free < file && d[free] < d[file])
            free += 2;
        if (free < file)
        {
            CHECK(d[file] <= d[free]);
            sum += sum_pos(pos[free], d[file]) * file / 2;
            /* std::println("* {} @ {} # {} -> {} ", file / 2, pos[free], d[file], sum); */
            d[free] -= d[file];
            pos[free] += d[file];
        }
        else
        {
            sum += sum_pos(pos[file], d[file]) * file / 2;
            /* std::println("= {} @ {} # {} -> {} ", file / 2, pos[file], d[file], sum); */
        }
    }
    return sum;
}


TEST_CASE("Sample")
{
    SUBCASE("Part 1")
    {
        auto disk = read_input("sample.txt");
        CHECK(defrag_checksum(disk) == 1928);
    }
    SUBCASE("Part 2")
    {
        auto disk = read_input("sample.txt");
        CHECK(defrag_by_file(disk) == 2858);
    }
}

TEST_CASE("Sample.2")
{
    SUBCASE("Part 1")
    {
        auto disk = from_line("12345");
        CHECK(defrag_checksum(disk) == 60);
    }
    SUBCASE("Part 2")
    {
        auto disk = from_line("12345");
        CHECK(defrag_by_file(disk) == 132);
    }
}


TEST_CASE("Input")
{
    SUBCASE("Part 1")
    {
        auto disk = read_input("input.txt");
        auto sum = defrag_checksum(disk);
        CHECK(sum == 6421128769094);
    }
    SUBCASE("Part 2")
    {
        auto disk = read_input("input.txt");
        CHECK(defrag_by_file(disk) == 6448168620520);
    }
}
