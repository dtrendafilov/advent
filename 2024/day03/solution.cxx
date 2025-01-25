#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"
#include "../darllen.hxx"

std::string load_input(const char* name)
{
    std::ifstream input(name);
    return std::string(std::istreambuf_iterator{input}, std::istreambuf_iterator<char>{});
}

int from_match(const auto& match)
{
    int result = 0;
    const bool ok = darllen::detail::to_value(match, result);
    CHECK(ok);
    return result;
}

int sum_mul(const std::string& input)
{
    const std::regex mul_re(R"(mul\(([[:digit:]]{1,3}),([[:digit:]]{1,3})\))");
    std::sregex_token_iterator beg(begin(input), end(input), mul_re, {1, 2}), end;
    int sum = 0;
    while (beg != end)
    {
        int first = from_match(*beg);
        ++beg;

        int second = from_match(*beg);
        ++beg;

        sum += first * second;
    }
    return sum;
}

int sum_mul_do(const std::string& input)
{
    const std::regex mul_re(
            R"((?:mul\(([[:digit:]]{1,3}),([[:digit:]]{1,3})\)))"
            R"(|(?:(do)\(\))|(?:(don't)\(\)))"
            );
    std::sregex_token_iterator beg(begin(input), end(input), mul_re, {1, 2, 3, 4}), end;
    int sum = 0;
    bool active = true;
    while (beg != end)
    {
        auto m1 = *beg++;
        auto m2 = *beg++;
        auto m3 = *beg++;
        auto m4 = *beg++;
        if (m3 == "do")
        {
            active = true;
        }
        else if (m4 == "don't")
        {
            active = false;
        }
        else if (active)
        {
            int first = from_match(m1);

            int second = from_match(m2);
            sum += first * second;
        }
    }
    return sum;
}


TEST_CASE("Regex")
{
    SUBCASE("Number")
    {
        std::regex num_re(R"([[:digit:]]{1,3})");
        std::string input("42");
        CHECK(std::regex_search(input, num_re));
    }
    SUBCASE("Numbers")
    {
        std::regex num_re(R"(([[:digit:]]{1,3}),([[:digit:]]{1,3}))");
        std::string input("42,42");
        CHECK(std::regex_search(input, num_re));
    }
    SUBCASE("Braces")
    {
        std::regex num_re(R"(\(([[:digit:]]{1,3}),([[:digit:]]{1,3})\))");
        std::string input("some(42,42)some");
        CHECK(std::regex_search(input, num_re));
    }
    SUBCASE("mul")
    {
        std::regex num_re(R"(mul\(([[:digit:]]{1,3}),([[:digit:]]{1,3})\))");
        std::string input("somemul(42,42)some");
        CHECK(std::regex_search(input, num_re));
    }
    SUBCASE("dodon't")
    {
        std::regex num_re(
                R"((?:mul\(([[:digit:]]{1,3}),([[:digit:]]{1,3})\)))"
                R"(|(?:(do)(\(\)))|(?:(don't)(\(\))))"
                );
        std::string input("somedo()some");
        CHECK(std::regex_search(input, num_re));
        CHECK(std::regex_search("domul(1,2)", num_re));
    }
}

TEST_CASE("Sample")
{
    SUBCASE("Part 1")
    {
        auto input = load_input("sample.txt");
        CHECK(sum_mul(input) == 161);

    }
    SUBCASE("Part 2")
    {
        auto input = load_input("sample2.txt");
        CHECK(sum_mul_do(input) == 48);
    }
}

TEST_CASE("Input")
{
    auto input = load_input("input.txt");
    SUBCASE("Part 1")
    {
        CHECK_EQ(sum_mul(input), 174336360);
    }
    SUBCASE("Part 2")
    {
        CHECK_EQ(sum_mul_do(input), 88802350);
    }
}
