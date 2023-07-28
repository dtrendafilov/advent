#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <ranges>
#include <format>

#include <cassert>

typedef int64_t Number;

void print(const auto& v)
{
    std::copy(begin(v), end(v), std::ostream_iterator<Number>(std::cout, ", "));
    std::cout << std::endl;
}


auto read(const char* source)
{
    std::ifstream input(source);
    std::ranges::istream_view<Number> numbers(input);
    std::vector<Number> result;
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

auto mix(std::span<Number> numbers, int mixes, int key)
{
    typedef std::list<Number> IntList;
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
    const auto modulo = numbers.size() - 1;
    const Number key_remind = Number(key) % modulo;
    std::cout << "modulo " << modulo << " key r " << key_remind << std::endl;

    for (int mix = 0; mix < mixes; ++mix)
    {
        /* for (auto number: std::views::zip(numbers, positions)) */
        for (auto i = 0u; i < numbers.size(); ++i)
        {
            Number skips = numbers[i] * key_remind;
            /* std::cout << numbers[i] << ' ' << skips << std::endl; */

            while (skips < 0)
            {
                skips += modulo;
            }
            skips %= modulo;
            if (!skips)
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

            for (auto i = skips; i > 0; --i)
            {
                ++new_position;
                while (new_position == result_end || new_position == next)
                {
                    ++new_position;
                }
            }
            result.splice(new_position, result, iterator);
        }
        /* print(result); */
    }
    return result;
}

Number groove(const std::list<Number>& mixed, int key)
{
    auto zero = std::find(begin(mixed), end(mixed), 0);
    auto i1000 = skip_advance(zero, 1000, end(mixed));
    std::cout << *i1000 * key << std::endl;
    auto i2000 = skip_advance(i1000, 1000, end(mixed));
    std::cout << *i2000 * key << std::endl;
    auto i3000 = skip_advance(i2000, 1000, end(mixed));
    std::cout << *i3000 * key << std::endl;
    return (*i1000 + *i2000 + *i3000) * key;
}

int main(int argc, const char* argv[])
{
    auto numbers = read(argc > 1? argv[1] : "input.txt");
    /* print(numbers); */
    {
        auto mixed = mix(numbers, 1, 1);
        /* print(mixed); */
        std::cout << groove(mixed, 1) << std::endl;
    }

    {
        auto mixed = mix(numbers, 10, 811589153);
        std::cout << groove(mixed, 811589153) << std::endl;
    }
    return 0;
}
