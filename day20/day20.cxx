#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <ranges>
#include <format>


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

    /* for (auto number: std::views::zip(numbers, positions)) */
    for (auto i = 0u; i < numbers.size(); ++i)
    {
        auto number = numbers[i];
        /* std::cout << number */
        /*     << " move with " << number + (number >= 0 ? 1 : -1) */
        /*     << std::endl; */
        auto iterator = positions[i];
        auto new_position = iterator;
        /* std::advance(new_position, number + (number >= 0 ? 1 : -1)); */
        auto result_end = end(result);
        auto result_begin = begin(result);
        if (number >= 0)
        {
            for (auto i = number + 1; i > 0; --i)
            {
                ++new_position;
                if (new_position == result_end)
                    ++new_position;
                /* std::cout << i << " at " << *new_position << std::endl; */
            }
        }
        else
        {
            for (auto i = number; i < 0; ++i)
            {
                --new_position;
                if (new_position == result_begin)
                    --new_position;
                /* std::cout << i << " at " << *new_position << std::endl; */
            }
        }
        result.splice(new_position, result, iterator);
        /* print(result); */
    }
    return result;
}

int groove(const std::list<int>& mixed)
{
    auto zero = std::find(begin(mixed), end(mixed), 0);
    auto i1000 = zero;
    std::advance(i1000, 1 + 1000 % mixed.size());
    std::cout << *i1000 << std::endl;
    auto i2000 = zero;
    std::advance(i2000, 1 + 2000 % mixed.size());
    std::cout << *i2000 << std::endl;
    auto i3000 = zero;
    std::advance(i3000, 1 + 3000 % mixed.size());
    std::cout << *i3000 << std::endl;
    return *i1000 + *i2000 + *i3000;
}

int main(int argc, const char* argv[])
{
    auto numbers = read(argc > 1? argv[1] : "input.txt");
    /* print(numbers); */
    auto mixed = mix(numbers);
    std::cout << groove(mixed) << std::endl;
    return 0;
}
