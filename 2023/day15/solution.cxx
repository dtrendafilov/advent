#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

int hash(std::string_view step)
{
    int digest = 0;
    for (auto c : step)
    {
        digest += c;
        digest *= 17;
        digest &= 0xff;
    }
    return digest;
}

std::string read_input(const char* name)
{
    std::ifstream file(name);
    std::string input;
    file >> input;
    return input;
}

int sum_hashes(std::string_view input)
{
    int sum = 0;
    for (auto command : std::views::split(input, ','))
    {
        sum += hash(std::string_view{command.begin(), command.end()});
    }
    return sum;
}

struct Lens
{
    std::string label;
    int length;
};
typedef std::vector<Lens> Box;
typedef std::array<Box, 256> Boxes;

Boxes fill_boxes(auto commands)
{
    Boxes boxes;
    for (auto command : commands)
    {
        const auto step = std::string_view{command.begin(), command.end()};
        auto action_index = step.find_first_of("-=");
        const auto action = step.begin() + action_index;
        const auto label_value = std::string_view{step.begin(), action};
        const auto label = hash(label_value);
        const auto same_label = [label_value](auto& lens) {
                    return lens.label == label_value;
        };
        auto& box = boxes[label];
        auto lens = std::find_if(box.begin(), box.end(), same_label);

        switch (*action)
        {
            case '-': {
                if (lens != box.end())
                {
                    box.erase(lens);
                }
                break;
            }
            case '=': {
                int length = 0;
                const char* first = &step[action_index + 1];
                const char* last = &step.back() + 1;
                std::from_chars(first, last, length);
                if (lens == box.end())
                {
                    box.push_back(Lens{std::string{label_value}, length});
                }
                else
                {
                    lens->length = length;
                }
                break;
            }
        }
    }
    return boxes;
}


uint64_t sum_boxes(const Boxes& boxes)
{
    uint64_t sum = 0;
    for (auto i = 0u; i < boxes.size(); ++i)
    {
        auto& box = boxes[i];
        for (auto j = 0u; j < box.size(); ++j)
        {
            sum += (i + 1) * (j + 1) * box[j].length;
        }
    }
    return sum;
}

TEST_CASE("hash")
{
    CHECK(hash("HASH") == 52);
    CHECK(hash("rn=1") == 30);
    SUBCASE("collision")
    {
        CHECK(hash("rn") == hash("cm"));
    }
}

TEST_CASE("Sample")
{
    auto input = read_input("sample.txt");
    SUBCASE("Part 1")
    {
        CHECK(sum_hashes(input) == 1320);

    }
    SUBCASE("Part 2")
    {
        auto boxes = fill_boxes(std::views::split(input, ','));
        CHECK(sum_boxes(boxes) == 145);
    }
}

TEST_CASE("Input")
{
    auto input = read_input("input.txt");
    SUBCASE("Part 1")
    {
        CHECK(sum_hashes(input) == 504036);
    }
    SUBCASE("Part 2")
    {
        auto boxes = fill_boxes(std::views::split(input, ','));
        CHECK(sum_boxes(boxes) == 295719);
    }
}
