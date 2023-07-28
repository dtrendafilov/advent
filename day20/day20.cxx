#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <ranges>
#include <format>

#include <cassert>


void print(const auto& v)
{
    std::copy(begin(v), end(v), std::ostream_iterator<int>(std::cout, ", "));
    std::cout << std::endl;
}


auto read(const char* source)
{
    std::ifstream input(source);
    std::ranges::istream_view<int> numbers(input);
    std::vector<int> result;
    std::ranges::copy(numbers, std::back_inserter(result));
    return result;
}

auto skip_advance(auto iterator, auto steps, auto end)
{
    auto result = iterator;
    for (auto i = steps; i > 0; --i)
    {
        ++result;
        if (result == end)
        {
            ++result;
        }
    }
    return result;
}

auto mix(std::span<int> numbers)
{
    typedef std::list<int> IntList;
    IntList result(begin(numbers), end(numbers));
    std::vector<IntList::iterator> positions;
    positions.reserve(numbers.size());

    for (auto i = begin(result); i != end(result); ++i)
    {
        positions.push_back(i);
    }

    {
        auto e = end(result);
        std::advance(e, 1);
        assert(e == begin(result));
        auto b = begin(result);
        std::advance(b, -1);
        assert(b == end(result));
    }

    const auto result_end = end(result);
    const auto result_begin = begin(result);
    const auto modulo = numbers.size() - 1;

    /* for (auto number: std::views::zip(numbers, positions)) */
    for (auto i = 0u; i < numbers.size(); ++i)
    {
        auto number = numbers[i];

        while (number < 0)
        {
            number += modulo;
        }
        number %= modulo;
        if (!number)
        {
            continue;
        }

        auto iterator = positions[i];
        /*
         * 1. Splicing at begin and end gives the same order, but consumes an
         * iterator increment, so skip the end iterator.
         * 2. Simulate removing the current number from the list (it doesn't
         * count to skip itself) by skipping the next eventual position.
         */
        auto next = iterator;
        ++next;
        if (next == result_end)
        {
            ++next;
        }
        auto new_position = iterator;

        for (auto i = number; i > 0; --i)
        {
            ++new_position;
            while (new_position == result_end || new_position == next)
            {
                ++new_position;
            }
        }
        result.splice(new_position, result, iterator);
    }
    return result;
}

int groove(const std::list<int>& mixed)
{
    auto zero = std::find(begin(mixed), end(mixed), 0);
    auto i1000 = skip_advance(zero, 1000, end(mixed));
    std::cout << *i1000 << std::endl;
    auto i2000 = skip_advance(i1000, 1000, end(mixed));
    std::cout << *i2000 << std::endl;
    auto i3000 = skip_advance(i2000, 1000, end(mixed));
    std::cout << *i3000 << std::endl;
    return *i1000 + *i2000 + *i3000;
}

int main(int argc, const char* argv[])
{
    auto numbers = read(argc > 1? argv[1] : "input.txt");
    /* print(numbers); */
    auto mixed = mix(numbers);
    /* print(mixed); */
    std::cout << groove(mixed) << std::endl;
    return 0;
}
