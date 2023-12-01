
import std.stdio;
import std.file;
import std.conv;
import std.range;
import std.algorithm;
import std.algorithm.iteration;
import std.algorithm.searching;

auto score(char item)
{
    return ((item <= 'Z')? (27 + item - 'A') : (1 + item - 'a'));
}

int racksack(string r)
{
    auto length = r.length;
    auto a = r[0..length / 2];
    auto b = r[length / 2 .. length];
    auto misplaced = a.findAmong(b);
    if (misplaced.length)
    {
        auto item = misplaced[0];
        /* auto count = misplaced.count(item).to!int; */
        return score(item);
    }
    return 0;
}

unittest
{
    assert(racksack("aZcZ") == 52);
    assert("abc".findAmong("def").length == 0);
    auto x = "p".findAmong("p");
    assert(x.length == 1);
    assert(x[0] == 'p');
    assert(racksack("pp") == 16);
    assert(racksack("apca") == 1);
    assert(racksack("zpbcaz") == 26);
    assert(racksack("aBBc") == 28);
    assert(racksack("vJrwpWtwJgWrhcsFMMfFFhFp") == 16);
    assert(racksack("jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL") == 38);
    assert(arrange("sample.txt") == 157);
    assert(groups("sample.txt") == 70);
}

int arrange(string name)
{
    return slurp!(string)(name, "%s")
        .map!racksack
        .sum;
}

struct Groups
{
    string[] _groups;
    int _start;

    bool empty() const {
        return _start >= _groups.length;
    }

    void popFront() {
        _start += 3;
    }

    auto front() const {
        return _groups[_start .. _start + 3];
    }
}

int group(const string[] g)
{
    string g13 = g[0];
    string g2 = g[1];
    do {
        g13 = g13.findAmong(g[2]);
        if (g2.canFind(g13[0]))
        {
            return score(g13[0]);
        }
        g13.popFront();
    } while (1);
    return score(g13[0]);
}

int groups(string name)
{
    auto result = slurp!(string)(name, "%s")
                .Groups
                .map!group
                .sum;
    return result;
}

int main()
{
    /* arrange("input.txt") */
    groups("input.txt")
        .writeln;
    return 0;
}
