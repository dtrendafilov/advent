import std.stdio;
import std.file;
import std.conv;
import std.algorithm;
import std.algorithm.iteration;
import std.range;


bool is_marker(string r)
{
    foreach (i; 0 .. r.length - 1)
    {
        foreach (j; i+1 .. r.length)
        {
            if (r[i] == r[j])
            {
                return false;
            }
        }
    }
    return true;
}

auto marker(string input, int window)
{
    int[char] occurancy;
    auto index = window;
    foreach (c; input[0..index])
    {
        occurancy.update(c, () => 1, (ref int v) { ++v; });
    }

    while (occurancy.length < window && index < input.length)
    {
        auto removed = input[index - window];
        occurancy.update(removed, () => 0, (ref int v) { --v; });
        if (occurancy[removed] == 0)
        {
            occurancy.remove(removed);
        }
        auto added = input[index];
        occurancy.update(added, () => 1, (ref int v) { ++v; });
        ++index;
    }
    return index;
}

int main()
{
    marker(readText("input.txt"), 14)
        .writeln;
    return 0;
}

unittest
{
    assert(marker("bvwbjplbgvbhsrlpgdmjqwftvncz", 4) == 5);
    assert(marker("nppdvjthqldpwncqszvftbrmjlhg", 4) == 6);
    assert(marker("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg", 4) == 10);
    assert(marker("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw", 4) == 11);

    assert(marker("mjqjpqmgbljsphdztnvjfqwrcgsmlb", 14) == 19);
    assert(marker("bvwbjplbgvbhsrlpgdmjqwftvncz", 14) == 23);
    assert(marker("nppdvjthqldpwncqszvftbrmjlhg", 14) == 23);
    assert(marker("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg", 14) == 29);
    assert(marker("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw", 14) == 26);
}


