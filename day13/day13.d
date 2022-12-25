import std.stdio;
import std.file;
import std.conv;
import std.algorithm;
import std.algorithm.iteration;
import std.format;
import std.range;
import std.string;


string[] parse(string v)
{
    string[] result;
    if (v[0] == '[')
    {
        assert(v[$ - 1] == ']');
        int balance = 0;
        ulong start = 1;
        foreach (index; 1 .. v.length - 1)
        {
            switch (v[index])
            {
                case '[': ++balance;
                          break;
                case ']': --balance;
                          assert(balance >= 0);
                          break;
                case ',':
                          if (balance == 0) {
                              result ~= v[start .. index];
                              start = index + 1;
                          }
                          break;
                default:
                    continue;
            }
        }
        result ~= v[start .. v.length - 1];
    }
    else
    {
        result = [v];
    }
    return result;
}


int cmp_order(string lhs, string rhs)
{
    int i = 0;
    auto lhs_s = parse(lhs);
    auto rhs_s = parse(rhs);
    int r = 0;
    while (r == 0 && i < lhs_s.length && i < rhs_s.length)
    {
        if (lhs_s[i].empty || rhs_s[i].empty)
        {
            r = lhs_s[i].length.to!int - rhs_s[i].length.to!int;
        }
        else if (lhs_s[i][0] != '[' && rhs_s[i][0] != '[')
        {
            r = lhs_s[i].to!int - rhs_s[i].to!int;
        }
        else
        {
            r = cmp_order(lhs_s[i], rhs_s[i]);
        }
        ++i;
    }

    return r != 0? r : lhs_s.length.to!int - rhs_s.length.to!int;
}

auto signals(T)(T input)
{
    return input.enumerate(1)
        .filter!((a) => cmp_order(a[1][0], a[1][1]) < 0)
        .map!((a) => a[0]);
}

auto split_input(string input)
{
    auto r = readText(input).splitLines
                    .splitter("");
    return r;
}

int decode(string[] input)
{
    input ~= "[[2]]";
    input ~= "[[6]]";
    input.sort!((l, r) => cmp_order(l, r) < 0);
    auto i = 1 + input.countUntil("[[2]]");
    auto j = 1 + input.countUntil("[[6]]");
    [i, j].writeln;
    return (i * j).to!int;
}

string[] sort_input(string input)
{
    return readText(input).splitLines.filter!((a) => !a.empty).array;
}


int main()
{
    /* split_input("input.txt").signals.sum.writeln; */
    sort_input("input.txt").decode.writeln;
    return 0;
}

unittest
{
    assert(parse("0") == ["0"]);
    assert(parse("[1]") == ["1"]);
    assert(parse("[1,2]") == ["1", "2"]);
    assert(parse("[[[1,2],3],4]") == ["[[1,2],3]", "4"]);

    assert(cmp_order("0", "0") == 0);
    assert(cmp_order("0", "1") == -1);
    assert(cmp_order("2", "1") == 1);
    assert(cmp_order("[2]", "1") == 1);
    assert(cmp_order("3", "[2,1]") == 1);
    auto dist = split_input("sample.txt").signals.array;
    /* dist.writeln; */
    assert(dist.sum == 13);

    auto s = sort_input("sample.txt").decode;
    assert(s == 140);
}



