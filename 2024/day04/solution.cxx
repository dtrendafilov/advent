#include "precompiled.hxx"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

std::vector<std::string> read_file(const char* name) {
    std::vector<std::string> lines;
    std::ifstream file(name);
    std::string line;
    
    std::getline(file, line);
    size_t max_length = line.length() + 2;  // +2 for the '#' borders
    
    lines.push_back(std::string(max_length, '#'));
    
    do {
        line = "#" + line + "#";
        lines.push_back(line);
    } while (std::getline(file, line));
    
    lines.push_back(std::string(max_length, '#'));
    return lines;
}

bool match_pattern(const std::vector<std::string>& lines, const std::string_view& pattern,
                  size_t row, size_t col, int drow, int dcol) {
    for (size_t i = 0, r = row, c = col; i < pattern.length(); i++, r += drow, c += dcol) {
        if (lines[r][c] != pattern[i]) {
            return false;
        }
    }
    return true;
}

size_t count_patterns(const std::vector<std::string>& lines, const std::string_view pattern) {
    size_t count = 0;
    
    const auto rows = lines.size() - 1;
    const auto cols = lines[0].length() - 1;
    for (size_t i = 1; i < rows; i++) {
        for (size_t j = 1; j < cols; j++) {
            if (match_pattern(lines, pattern, i, j, 0, 1))  count++; // right
            if (match_pattern(lines, pattern, i, j, 1, 0))  count++; // down
            if (match_pattern(lines, pattern, i, j, 1, 1))  count++; // down-right
            if (match_pattern(lines, pattern, i, j, 1, -1)) count++; // down-left
        }
    }
    return count;
}

size_t count_words(const std::vector<std::string>& lines, const std::string_view word) {
    auto forward = count_patterns(lines, word);
    auto reverse = count_patterns(lines, std::string(word.rbegin(), word.rend()));
    return forward + reverse;
}

int count_x_patterns(const std::vector<std::string>& lines, const std::string_view pattern) {
    CHECK(pattern.length() % 2 == 1);
    auto reversed = std::string(pattern.rbegin(), pattern.rend());

    int count = 0;
    
    const auto rows = lines.size() - pattern.length();
    const auto cols = lines[0].length() - pattern.length();
    for (size_t r = 1; r < rows; r++) {
        for (size_t c = 1; c < cols; c++) {
            if ((match_pattern(lines, pattern, r, c, 1, 1) || match_pattern(lines, reversed, r, c, 1, 1))
             && (match_pattern(lines, pattern, r, c + pattern.length() - 1, 1, -1) || match_pattern(lines, reversed, r, c + pattern.length() - 1, 1, -1))) {
                count++;
            }            
        }
    }
    return count;
}

TEST_CASE("Sample")
{
    auto input = read_file("sample.txt");
    SUBCASE("Part 1")
    {
        CHECK(count_words(input, "XMAS") == 18);

    }
    SUBCASE("Part 2")
    {
        CHECK(count_x_patterns(input, "MAS") == 9);
    }
}

TEST_CASE("Input")
{
    auto input = read_file("input.txt");
    SUBCASE("Part 1")
    {
        CHECK(count_words(input, "XMAS") == 2549);

    }
    SUBCASE("Part 2")
    {
        CHECK(count_x_patterns(input, "MAS") == 2003);
    }
}
