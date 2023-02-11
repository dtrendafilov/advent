import std.stdio;
import std.file;
import std.conv;
import std.range;
import std.string : splitLines;
import std.algorithm : map, countUntil, sum, sort, uniq, equal;


int[] numbers(string input)
{
    auto n = readText(input).splitLines.map!((a) => a.to!int).array;
    return n;
}

int[] encrypt(int[] n)
{
    int length = n.length.to!int;
    auto indices = iota(length).map!(to!int).array;
    foreach (j; 0..length)
    {
        auto x = n[j];
        if (x == 0)
        {
            continue;
        }
        int c = indices[j];
        int t = (c + x) % length;
        /* if (t == c) */
        /* { */
        /*     continue; */
        /* } */
        if (t <= 0)
        {
            t += length;
            if (t >= c)
                --t;
        }
        if (t < c)
        {
            if (x > 0)
            {
                ++t;
            }
            foreach (ref i; indices)
            {
                if (t <= i && i < c)
                {
                    ++i;
                }
            }
        }
        else
        {
            foreach (ref i; indices)
            {
                if (c < i && i <= t)
                {
                    --i;
                }
            }
        }
        indices[j] = t;
        /* indices.writeln; */
    }
    assert(no_repeats(indices));
    auto c = new int[length];
    foreach (j; 0..length)
    {
        c[indices[j]] = n[j]; 
    }
    return c;
}

bool no_repeats(int[] indices)
{
    auto u = indices.dup.sort.uniq;
    return indices.length == u.walkLength;
}

auto coords(int[] n)
{
    auto z = n.countUntil(0);
    return [1000, 2000, 3000].map!((c) => n[(z + c) % n.length]).sum;
}


int main()
{
    auto n = numbers("input.txt").encrypt;
    coords(n).writeln;
    return 0;
}

unittest
{
    assert(equal(encrypt([ 1, 0, 0, 0, 0]), [ 0, 1, 0, 0, 0 ]));
    assert(equal(encrypt([-1, 0, 0, 0, 0]), [ 0, 0, 0, -1, 0 ]));
    assert(equal(encrypt([ 2, 0, 0, 0, 0]), [ 0, 0, 2, 0, 0 ]));
    assert(equal(encrypt([-2, 0, 0, 0, 0]), [ 0, 0, -2, 0, 0 ]));
    assert(equal(encrypt([ 3, 0, 0, 0, 0]), [ 0, 0, 0, 3, 0 ]));
    assert(equal(encrypt([-3, 0, 0, 0, 0]), [ 0, -3, 0, 0, 0 ]));
    assert(equal(encrypt([ 4, 0, 0, 0, 0]), [ 0, 0, 0, 0, 4 ]));
    assert(equal(encrypt([-4, 0, 0, 0, 0]), [-4, 0, 0, 0, 0 ]));
    encrypt([5, 0, 0, 0, 0]).writeln;
    assert(equal(encrypt([ 5, 0, 0, 0, 0]), [ 0, 0, 0, 0, 5 ])); // ???

    encrypt([-5, 0, 0, 0, 0]).writeln;
    assert(equal(encrypt([-5, 0, 0, 0, 0]), [ 0, 0, 0, 0, -5 ])); // ???

    auto n = numbers("sample.txt").encrypt;
    auto z = n.countUntil(0);
    [1000, 2000, 3000].map!((c) => n[(z + c) % n.length]).array.writeln;
    assert(coords(n) == 3);
}
