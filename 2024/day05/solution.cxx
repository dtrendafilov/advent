#include "precompiled.hxx"

#include "../darllen.hxx"

#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

using Node = int;
const Node MAX_NODE = 99;

struct Edge
{
    Node from;
    Node to;

    std::strong_ordering operator<=>(const Edge& rhs) const = default;
};
using Edges = std::vector<Edge>;

using Nodes = std::vector<Node>;

struct Graph
{
    Edges edges;

    std::vector<Nodes> _follows;

    void compute_follows()
    {
        _follows.resize(MAX_NODE + 1);

        std::ranges::sort(edges);

        for (Node n = 0; n <= MAX_NODE; ++n)
        {
            _follows[n] = bfs(n);
        }
    }

    auto edges_from_node(Node n)
    {
        /* auto edges_begin = std::ranges::lower_bound(edges, Edge{n, 0}); */
        /* auto edges_end = std::ranges::upper_bound(edges, Edge{n, MAX_NODE}); */
        /* return std::make_tuple(edges_begin, edges_end); */
        return std::ranges::equal_range(edges, n, std::less{}, &Edge::from);
    }

    auto nodes_from_node(Node n)
    {
        auto get_to = [](const Edge& e) { return e.to; };
        return edges_from_node(n) | std::ranges::views::transform(get_to);
    }
    
    Nodes bfs(Node n)
    {
        Nodes result;
        result.reserve(MAX_NODE);
        Nodes merged;
        merged.reserve(MAX_NODE);
        std::queue<Node> wave;

        wave.push(n);

        std::array<bool, MAX_NODE + 1> visited{};


        while (!wave.empty())
        {
            auto to = wave.front();
            wave.pop();
            visited[to] = true;

            std::ranges::set_union(result, nodes_from_node(to), std::back_inserter(merged));
            swap(result, merged);
            merged.clear();

            // Turns out the relation does not have to be considered transitive
            /*
            for (auto next : nodes_from_node(to))
            {
                if (!visited[next])
                {
                    wave.push(next);
                }
            }
            */
        }

        return result;
    }

    void dfs(Node node)
    {
        if (!_follows[node].empty())
        {
            return;
        }
        Nodes result;
        result.reserve(MAX_NODE);
        Nodes merged;
        merged.reserve(MAX_NODE);
        for (auto to : nodes_from_node(node))
        {
            result.insert(std::ranges::lower_bound(result, to), to);

            dfs(to);

            const auto& next = _follows[to];
            std::ranges::set_union(result, next, std::back_inserter(merged));
            swap(merged, result);
            merged.clear();

        }
        _follows[node] = std::move(result);
    }

    bool follows(Node from, Node to) const {
        return std::ranges::binary_search(_follows[from], to);
    }
};

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
        // NOTE: Reversing the direction of the edge
        if (darllen::read(edge_re, begin(line), end(line), edge.to, edge.from))
        {
            graph.edges.push_back(edge);
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
    CHECK_EQ(graph.edges.size(), 6);
    CHECK_EQ(prints.size(), 8);
}

bool is_valid_print(const Graph& g, const Print& print)
{
    for (auto [f, t] : print | std::ranges::views::adjacent<2>)
    {
        if (g.follows(f, t))
        {
            return false;
        }
    }
    return true;
}

int sum_valid_prints(const Graph& g, const Prints& prints)
{
    int sum = 0;
    auto is_valid = [g](const Print& print) { return is_valid_print(g, print); };

    for (auto& v : prints | std::ranges::views::filter(is_valid))
    {
        CHECK(v.size() % 2 == 1);
        sum += v[v.size() / 2];
    }

    return sum;
}

auto invalid_prints(const Graph& g, const Prints& prints)
{
    auto is_invalid = [g](const Print& print) { return !is_valid_print(g, print); };
    return prints | std::ranges::views::filter(is_invalid);
}

int fix_print(const Graph& g, Print print)
{
    for (auto i = 1u; i < print.size(); ++i)
    {
        if (g.follows(print[i-1], print[i]))
        {
            std::swap(print[i-1], print[i]);
            i = 0;
        }
    }
    return print[print.size() / 2];
}

int sum_fixed_prints(const Graph& g, const Prints& prints)
{
    int sum = 0;
    for (auto invalid : invalid_prints(g, prints))
    {
        sum += fix_print(g, invalid);
    }
    return sum;
}

TEST_CASE("Sample")
{
    std::ifstream input("sample.txt");
    auto [graph, prints] = read_input(input);
    graph.compute_follows();
    CHECK(graph.follows(47, 75));
    CHECK(graph.follows(47, 97));
    CHECK(is_valid_print(graph, prints[0]));
    CHECK(is_valid_print(graph, prints[1]));
    CHECK(is_valid_print(graph, prints[2]));
    SUBCASE("Part 1")
    {
        CHECK(sum_valid_prints(graph, prints) == 143);

    }
    SUBCASE("Part 2")
    {
        CHECK(sum_fixed_prints(graph, prints) == 123);
    }
}

TEST_CASE("Input")
{
    std::ifstream input("input.txt");
    auto [graph, prints] = read_input(input);
    graph.compute_follows();
    SUBCASE("Part 1")
    {
        CHECK(sum_valid_prints(graph, prints) == 4609);
    }
    SUBCASE("Part 2")
    {
        CHECK(sum_fixed_prints(graph, prints) == 5723);
    }
}

#if !defined(DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)

int main(int argc, const char* argv[])
{
    std::ifstream input((argc > 1)? argv[1] : "sample.txt");
    auto [graph, prints] = read_input(input);
    graph.compute_follows();
    std::println("47 75 {}", graph.follows(47, 75));
    std::println("75 47 {}", graph.follows(75, 47));
    return 0;
}

#endif
