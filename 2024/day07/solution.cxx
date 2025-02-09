#include "precompiled.hxx"
#include <cstdint>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

#include "../darllen.hxx"

using Number = int64_t;

struct Equation
{
    Number value;
    std::vector<Number> numbers;
};

std::vector<Equation> read_input(const char* name)
{
    std::ifstream input(name);

    std::vector<Equation> r;
    std::string line;
    std::regex number_re{"([[:digit:]]+)"};
    int number;
    while (getline(input, line))
    {
        Equation e;

        std::sregex_token_iterator beg(begin(line), end(line), number_re, {1}), end;
        darllen::from_match(*beg, e.value);
        for (++beg; beg != end; ++beg)
        {
            if (darllen::from_match(*beg, number))
            {
                e.numbers.push_back(number);
            }
        }
        r.emplace_back(std::move(e));     
    }
    return r;
}

Number concat(Number lhs, Number rhs)
{
    auto l = rhs;
    while (l) {
        lhs *= 10;
        l /= 10;
    }
    return lhs + rhs;
}

bool is_possible(Number result, Number so_far, std::span<const Number> numbers)
{
    if (numbers.empty())
    {
        return result == so_far;
    }

    auto first = begin(numbers);
    return is_possible(result, so_far + *first, numbers.subspan(1)) 
        || is_possible(result, so_far * *first, numbers.subspan(1));
}

bool is_possible2(Number result, Number so_far, std::span<const Number> numbers)
{
    if (numbers.empty())
    {
        return result == so_far;
    }

    auto first = begin(numbers);
    return is_possible2(result, so_far + *first, numbers.subspan(1)) 
        || is_possible2(result, so_far * *first, numbers.subspan(1))
        || is_possible2(result, concat(so_far, *first), numbers.subspan(1));
}

bool is_possible_eq(const Equation& eq)
{
    std::span s{eq.numbers}; 
    return is_possible(eq.value, *begin(s), s.subspan(1));
}

bool is_possible_eq2(const Equation& eq)
{
    std::span s{eq.numbers}; 
    return is_possible2(eq.value, *begin(s), s.subspan(1));
}

Number possible(std::span<Equation> equations, auto check)
{
    Number sum = 0;
    for (auto p : equations | std::ranges::views::filter(check))
    {
        /* CHECK(sum + p.value >= sum); */
        sum += p.value;
    }
    return sum;
}


TEST_CASE("Sample")
{
    auto equations = read_input("sample.txt");
    SUBCASE("Part 1")
    {
        CHECK(possible(equations, is_possible_eq) == 3749);
    }
    SUBCASE("Part 2")
    {
        CHECK(possible(equations, is_possible_eq2) == 11387);
    }
}

TEST_CASE("Input")
{
    CHECK(concat(15, 6) == 156);
    auto equations = read_input("input.txt");
    SUBCASE("Part 1")
    {
        CHECK(possible(equations, is_possible_eq) == 7885693428401);
    }
    SUBCASE("Part 2")
    {
        CHECK(possible(equations, is_possible_eq2) == 348360680516005);
    }
}
