#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <charconv>

typedef std::vector<std::string> Schematic;

Schematic read_schematic(const char* input_name)
{
    using namespace std;

    ifstream input(input_name);
    Schematic schematic;

    string line;

    schematic.push_back(line);
    while (getline(input, line))
    {
        schematic.emplace_back("."s + line + "."s);
    }
    // add guards
    auto width = schematic.back().size();
    schematic.front().resize(width, '.');
    auto bottom = schematic.front();
    schematic.emplace_back(bottom);

    return schematic;
}

constexpr bool is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

constexpr bool is_symbol(char c)
{
    return c != '.';
}


int get_part_number(const Schematic& schematic, size_t row, size_t& col)
{
    int number = 0;

    auto start_col = col;
    auto line = &schematic[row][col];
    auto result = std::from_chars(line, line + schematic[row].size() - col,
                                  number);
    col = start_col + size_t(result.ptr - line);


    if (is_symbol(schematic[row][start_col - 1])
            || is_symbol(schematic[row][col]))
    {
        return number;
    }

    for (auto i = row - 1; i <= row + 1; i += 2)
    {
        auto& schema_row = schematic[i];
        if (std::any_of(&schema_row[start_col - 1], &schema_row[col + 1],
                    is_symbol))
        {
            return number;
        }
    }
    return 0;
}

int sum_part_numbers(const Schematic& schematic)
{
    auto last_row = schematic.size() - 2;
    auto last_col = schematic[0].size() - 2;
    int sum = 0;
    for (size_t i = 1u; i <= last_row; ++i)
    {
        auto& schema_row = schematic[i];
        for (size_t j = 1u; j <= last_col; ++j)
        {
            if (is_digit(schema_row[j]))
            {
                sum += get_part_number(schematic, i, j);
            }
        }
    }
    return sum;
}

int get_gear_ratio(const Schematic& schematic, size_t row, size_t col)
{
    int ratio = 1;
    int numbers = 0;
    for (auto i = row - 1; i <= row + 1; ++i)
    {
        for (auto j = col - 1; j <= col + 1; ++j)
        {
            if (is_digit(schematic[i][j]))
            {
                while (is_digit(schematic[i][j - 1])) --j;

                ratio *= get_part_number(schematic, i, j);
                ++numbers;
            }
        }
    }
    return (numbers == 2)? ratio : 0;
}

int sum_gear_ratios(const Schematic& schematic)
{
    int sum = 0;

    auto last_row = schematic.size() - 2;
    auto last_col = schematic[0].size() - 2;
    for (size_t i = 1u; i <= last_row; ++i)
    {
        auto& schema_row = schematic[i];
        for (size_t j = 1u; j <= last_col; ++j)
        {
            if (schema_row[j] == '*')
            {
                sum += get_gear_ratio(schematic, i, j);
            }
        }
    }

    return sum;
}


int main(int argc, const char* argv[])
{
    using namespace std;
    if (argc != 3) {
        cerr << "Usage: solution part input" << endl;
        return 1;
    }
    char part = *argv[1];
    auto input = argv[2];

    cout << part << ": " << input << endl;

    auto schematic = read_schematic(input);

    switch (part)
    {
        case '1':
        {
            cout << sum_part_numbers(schematic) << endl;
            break;
        }
        case '2':
        {
            cout << sum_gear_ratios(schematic) << endl;
            break;
        }
        default:
            cerr << "Unknown part: " << part << endl;
    }
    return 0;
}
