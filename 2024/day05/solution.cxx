#include "precompiled.hxx"

#include "../darllen.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

struct Edge
{
    int from;
    int to;
};

using Graph = std::vector<Edge>;

using Print = std::vector<int>;
using Prints = std::vector<Print>;

auto read_input(std::istream& input)
{
    Graph graph;
    Prints prints;

    std::string line;

    std::regex edge_re{R"(([[:digit:]]+)\|([[:digit:]]+))"};

    while (std::getline(input, line))
    {
        Edge edge;
        if (darllen::read(edge_re, begin(line), end(line), edge.from, edge.to))
        {
            graph.push_back(edge);
        }
        else
        {
            break;
        }
    }

    // skip the empty line

    std::regex print_re{"([[:digit:]]+)"};
    while (std::getline(input, line))
    {
        Print print;
        int page;
        std::sregex_token_iterator beg(begin(line), end(line), print_re, {1}), end;
        while (beg != end)
        {
            if (darllen::from_match(*beg, page))
            {
                print.push_back(page);
            }
            ++beg;
        }
        prints.emplace_back(std::move(print));     
    }

    return std::make_tuple(graph, prints);
}

TEST_CASE("Regex")
{
    SUBCASE("Edge")
    {
        std::regex edge_re{R"([[:digit:]]+\|[[:digit:]]+)"};
        CHECK(std::regex_match("1|2", edge_re));
        CHECK(std::regex_match("42|42", edge_re));
    }
}

TEST_CASE("Input")
{
    std::string input = "5|6\n"
                        "1|2\n"
                        "2|3\n"
                        "3|4\n"
                        "4|5\n"
                        "5|1\n"
                        "\n"
                        "1,2 3 4 5 6\n"
                        "1,2 3 4 5 6\n"
                        "1,2 3 4 5 6\n"
                        "2,3 4 5 6 1\n"
                        "3,4 5 6 1 2\n"
                        "4,5 6 1 2 3\n"
                        "5,6 1 2 3 4\n"
                        "6,1,2 3 4 5\n";
    std::istringstream input_stream(input);
    auto [graph, prints] = read_input(input_stream);
    CHECK_EQ(graph.size(), 6);
    CHECK_EQ(prints.size(), 8);
}

TEST_CASE("Sample")
{
    std::ifstream input("sample.txt");
    auto [graph, prints] = read_input(input);
    SUBCASE("Part 1")
    {
        CHECK(6 * 7 == 42);

    }
    SUBCASE("Part 2")
    {
        CHECK(6 + 7 == 13);
    }
}

TEST_CASE("Input")
{
    SUBCASE("Part 1")
    {
        CHECK(6 * 7 == 42);
    }
    SUBCASE("Part 2")
    {
        CHECK(6 + 7 == 13);
    }
}
