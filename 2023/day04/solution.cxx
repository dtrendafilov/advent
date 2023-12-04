#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>
#include <charconv>
#include <cassert>

const char* skip_ws(const char* p, const char* end)
{
    while (p != end && *p == ' ') ++p;
    return p;
}

std::vector<int> read_numbers(std::string_view text)
{
    auto numbers_begin = &text[0];
    auto numbers_end = numbers_begin + text.size();

    numbers_begin = skip_ws(numbers_begin, numbers_end);

    std::vector<int> numbers;
    int number = 0;
    while (numbers_begin < numbers_end)
    {
        auto result = std::from_chars(numbers_begin, numbers_end, number);
        // skip number and trailing space
        // keep program well-defined???
        numbers_begin = skip_ws(result.ptr, numbers_end);

        numbers.push_back(number);
    }
    std::sort(begin(numbers), end(numbers));
    return numbers;
}

int card_matches(std::string_view card)
{
    auto column = card.find(':');
    assert(column != std::string_view::npos);
    auto pipe = card.find('|');
    assert(pipe != std::string_view::npos);
    auto winning = read_numbers(card.substr(column + 1, pipe - column - 2));
    auto present = read_numbers(card.substr(pipe + 2));

    auto matches = winning;

    auto matches_end = std::set_intersection(begin(winning), end(winning),
            begin(present), end(present), begin(matches));
    matches.erase(matches_end, end(matches));


    return matches.size();
}

int points_total(const char* input_name)
{
    std::fstream input(input_name);
    std::string card;

    int points = 0;
    while (std::getline(input, card))
    {
        auto matches = card_matches(card);
        points += matches? (1 << (matches - 1)) : 0;
    }
    return points;
}

int cards_total(const char* input_name)
{
    std::fstream input(input_name);
    std::string card;

    std::vector<int> cards(1, 1);
    auto index = 0u;
    while (std::getline(input, card))
    {
        ++index;
        auto points = card_matches(card);
        auto size = index + points;
        if (cards.size() < size)
        {
            cards.resize(size, 1);
        }
        for (auto n = index; n < size; ++n)
        {
            cards[n] += cards[index - 1];
        }
    }
    return std::accumulate(begin(cards), end(cards), 0);
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


    switch (part)
    {
        case '1':
        {
            cout << points_total(input) << endl;
            break;
        }
        case '2':
        {
            cout << cards_total(input) << endl;
            break;
        }
        default:
            cerr << "Unknown part: " << part << endl;
    }
    return 0;
}
