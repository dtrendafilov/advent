#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

using Designs = std::vector<std::string>;

auto read_input(const char* filename)
{
    std::ifstream file(filename);
    
    std::string line;
    std::string regex_text{"(?:(?:"};
    {
        std::getline(file, line);
        std::regex_replace(std::back_inserter(regex_text), line.begin(), line.end(), std::regex{"(, )"}, ")|(?:");
        regex_text += "))+";
    }

    Designs lines;
    do
    {
        std::getline(file, line);
    } 
    while (line.empty());

    do
    {
        lines.push_back(line);
    }
    while (std::getline(file, line));

    return std::make_tuple(std::regex{regex_text}, lines);
}

auto possible_designs(const std::regex& stripes, const Designs& designs)
{
    return std::count_if(designs.begin(), designs.end(), [&](const auto& design)
    {
        return std::regex_match(design, stripes);
    });
}

TEST_CASE("Regex")
{
    std::regex regex{"(?:(?:r)|(?:wr)|(?:b)|(?:g)|(?:bwu)|(?:rb)|(?:gb)|(?:br))+"};
    CHECK(std::regex_match("r", regex));
    CHECK(std::regex_match("bwu", regex));
    CHECK(std::regex_match("rgb", regex));
    CHECK(std::regex_match("rwr", regex));
}

TEST_CASE("Sample")
{
    auto [stripes, designs] = read_input("sample.txt");
    SUBCASE("Part 1")
    {
        CHECK(possible_designs(stripes, designs) == 6);

    }
    SUBCASE("Part 2")
    {
        CHECK(6 + 7 == 13);
    }
}

TEST_CASE("Input")
{
    auto [stripes, designs] = read_input("input.txt");
    SUBCASE("Part 1")
    {
        CHECK(possible_designs(stripes, designs) == 242);
    }
    SUBCASE("Part 2")
    {
        CHECK(6 + 7 == 13);
    }
}
