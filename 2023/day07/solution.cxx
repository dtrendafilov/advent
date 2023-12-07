#include <array>
#include <charconv>
#include <compare>
#include <cstdint>
#include <functional>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <string_view>
#include <vector>
#include <string>


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

constexpr uint8_t card_strength(char card)
{
    switch (card)
    {
        case '2': return 0;
        case '3': return 1;
        case '4': return 2;
        case '5': return 3;
        case '6': return 4;
        case '7': return 5;
        case '8': return 6;
        case '9': return 7;
        case 'T': return 8;
        case 'J': return 9;
        case 'Q': return 10;
        case 'K': return 11;
        case 'A': return 12;
    }
    return 0;
}

constexpr uint8_t card_strength_joker(char card)
{
    switch (card)
    {
        case '2': return 1;
        case '3': return 2;
        case '4': return 3;
        case '5': return 4;
        case '6': return 5;
        case '7': return 6;
        case '8': return 7;
        case '9': return 8;
        case 'T': return 9;
        case 'J': return 0;
        case 'Q': return 10;
        case 'K': return 11;
        case 'A': return 12;
    }
    return 0;
}

typedef std::array<uint8_t, 5> Hand;


typedef std::array<uint8_t, 13> CardsCount;

int hand_type(const Hand& hand, bool joker)
{
    CardsCount counts{};
    for (auto c : hand)
    {
        ++counts[c];
    }
    if (joker)
    {
        constexpr auto joker_strength = card_strength_joker('J');
        if (counts[joker_strength] > 0 && counts[joker_strength] < 5)
        {
            const auto jokers = counts[joker_strength];
            counts[joker_strength] = 0;
            auto max = std::max_element(begin(counts), end(counts));
            *max += jokers;
        }
    }
    std::sort(begin(counts), end(counts), std::greater<uint8_t>{});

    switch (counts[0])
    {
        case 1: return 0;
        case 2: return counts[1] == 1? 1 : 2;
        case 3: return counts[1] == 1? 3 : 4;
        case 4: return 5;
        case 5: return 6;
    }
    CHECK(false);
    return 0;
}

Hand create_hand(std::string_view text_hand, bool joker)
{
    Hand hand;
    if (joker)
    {
        std::transform(begin(text_hand), end(text_hand), begin(hand),
                card_strength_joker);
    }
    else {
        std::transform(begin(text_hand), end(text_hand), begin(hand),
                card_strength);
    }
    return hand;
}

TEST_CASE("Answer is sane")
{
    CHECK(6 * 7 == 42);
}

TEST_CASE("Score hand")
{
    CHECK(hand_type(create_hand("23457", false), false) == 0);
    CHECK(hand_type(create_hand("23452", false), false) == 1);
    CHECK(hand_type(create_hand("23432", false), false) == 2);
    CHECK(hand_type(create_hand("22432", false), false) == 3);
    CHECK(hand_type(create_hand("23322", false), false) == 4);
    CHECK(hand_type(create_hand("22232", false), false) == 5);
    CHECK(hand_type(create_hand("22222", false), false) == 6);
}

TEST_CASE("Score hand with joker")
{
    CHECK(hand_type(create_hand("23457", true), true) == 0);
    CHECK(hand_type(create_hand("J3457", true), true) == 1);
    CHECK(hand_type(create_hand("J345J", true), true) == 3);
    CHECK(hand_type(create_hand("2J452", true), true) == 3);
    CHECK(hand_type(create_hand("2J252", true), true) == 5);
    CHECK(hand_type(create_hand("23J32", true), true) == 4);
    CHECK(hand_type(create_hand("23JJ2", true), true) == 5);
    CHECK(hand_type(create_hand("J2J22", true), true) == 6);
    CHECK(hand_type(create_hand("J2JJ2", true), true) == 6);
    CHECK(hand_type(create_hand("2JJJ3", true), true) == 5);
    CHECK(hand_type(create_hand("JJJJJ", true), true) == 6);
    CHECK(hand_type(create_hand("3JJJJ", true), true) == 6);
}

struct HandBid
{
    HandBid(std::string_view text_hand, int b, bool joker)
        : hand(create_hand(text_hand, joker))
        , bid(b)
        , type(hand_type(hand, joker))
    {
    }
    Hand hand;
    int bid;
    int type;

    std::strong_ordering operator <=>(const HandBid& rhs) const
    {
        if (auto cmp = type <=> rhs.type; cmp != 0)
        {
            return cmp;
        }
        return hand <=> rhs.hand;
    }
};

std::vector<HandBid> read_game(const char* input_name, bool joker)
{
    std::vector<HandBid> game;
    std::fstream input(input_name);
    std::string hand;
    int bid;
    while (input >> hand >> bid)
    {
        game.emplace_back(HandBid(hand, bid, joker));
    }
    std::sort(begin(game), end(game));

    for (auto i = 1u; i < game.size(); ++i)
    {
        CHECK(game[i - 1] < game[i]);
    }

    return game;
}

uint64_t compute_winnings(const std::vector<HandBid>& game)
{
    uint64_t winnings = 0u;
    for (auto i = 0u; i < game.size(); ++i)
    {
        winnings += game[i].bid * (i + 1);
    }
    return winnings;
}

TEST_CASE("Sample")
{
    SUBCASE("Part 1")
    {
        auto game = read_game("sample.txt", false);
        auto winnings = compute_winnings(game);
        CHECK(winnings == 6440);
        std::cout << "Sample 1: " << winnings << std::endl;
    }

    SUBCASE("Part 2")
    {
        auto game = read_game("sample.txt", true);
        auto winnings = compute_winnings(game);
        CHECK(winnings == 5905);
        std::cout << "Sample 2: " << winnings << std::endl;
    }
}

TEST_CASE("Input")
{
    SUBCASE("Part 1")
    {
        auto game = read_game("input.txt", false);
        auto winnings = compute_winnings(game);
        CHECK(winnings == 248396258);
        std::cout << "Input 1: " << winnings << std::endl;
    }
    SUBCASE("Part 2")
    {
        auto game = read_game("input.txt", true);
        auto winnings = compute_winnings(game);
        CHECK(winnings == 246436046);
        std::cout << "Input 2: " << winnings << std::endl;
    }
}
