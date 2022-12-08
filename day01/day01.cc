#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstdlib>
#include <ranges>

int main()
{
    std::vector<int> calories;
    int current = 0;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (!line.empty()) {
            current += atoi(line.c_str());
        } else {
            calories.push_back(current);
            current = 0;
        }
    }

    auto b = calories.begin();
    std::ranges::nth_element(calories, b + 3, std::greater<int>());
    auto s = std::accumulate(b, b + 3, 0);
    std::cout << s << std::endl;
}
