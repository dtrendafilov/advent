#include <iostream>
#include <fstream>
#include <ranges>
#include <string>
#include <string_view>
#include <cassert>
#include <array>


int read_map(const char* input, auto decode)
{
    using namespace std;

    ifstream input_file(input);
    auto lines = ranges::istream_view<string>(input_file);
    auto code = lines | views::transform(decode);
    // no sum :(
    auto sum = 0;
    for (auto n : code) {
        /* cerr << n << endl; */
        sum += n;
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

    cout << input << endl;
    if (part == '1')
    {
        auto decoder = [](string_view line) {
            auto f = line.find_first_of("0123456789");
            assert(f != string::npos);
            auto l = line.find_last_of("0123456789");
            assert(l != string::npos);
            return (line[f] - '0') * 10 + (line[l] - '0');
        };
        cout << read_map(input, decoder) << endl;
    }
    else if (part == '2')
    {
        std::array<std::string, 18> digits{
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "one",
            "two",
            "three",
            "four",
            "five",
            "six",
            "seven",
            "eight",
            "nine",
        };
        auto decoder = [digits](string_view line) {
            // no enumerate :(
            
            size_t first_position = line.length();
            int first_value;
            // there might be a single digit per line,
            // so last_position may be 0
            size_t last_position = string::npos;
            int last_value;

            for (auto i = 0u; i < digits.size(); ++i)
            {
                auto found = line.find(digits[i]);
                if (found != string::npos && found < first_position)
                {
                    first_position = found;
                    first_value = i;
                }
                found = line.rfind(digits[i]);
                if (found != string::npos && 
                        (found >= last_position || last_position == string::npos))
                {
                    last_position = found;
                    last_value = i;
                }
            }
            assert(first_position < line.length());
            if (last_position == 0) {
                last_position = first_position;
                last_value = first_value;
            }
            assert(last_position != string::npos);
            return (first_value % 9 + 1) * 10 + (last_value % 9 + 1);
        };
        cout << read_map(input, decoder) << endl;
    }
    else
    {
        cerr << "Unknown part: " << part << endl;
    }



    return 0;
}
