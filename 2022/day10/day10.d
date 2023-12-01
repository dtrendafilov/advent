import std.stdio;
import std.file;
import std.conv;
import std.algorithm;
import std.algorithm.iteration;
import std.range;
import std.format;
import std.string;
import std.math;


auto signals(string[] input)
{
    int x = 1;
    int cycle = 1;
    int check = 20;
    int v;
    int[] values;
    values.reserve(input.length / 40);

    foreach (cmd; input)
    {
        if (cmd == "noop")
        {
            ++cycle;
        }
        else
        {
            cmd.formattedRead("addx %s", v);
            cycle += 2;
            if (cycle > check)
            {
                values ~= check * x;
                check += 40;
            }
            x += v;
        }
    }
    return values;
}

auto draw(string[] input)
{
    int x = 1;
    int v;
    int c = 0;

    char[] pixels;


    foreach (cmd; input)
    {
        if (cmd == "noop")
        {
            ++c;
            auto pixel = ((x - pixels.length.to!int % 40).abs <= 1)? '#' : '.';
            pixels ~= pixel;
        }
        else
        {
            foreach (i; 0..2)
            {
                int d = x - (pixels.length.to!int % 40);
                auto pixel = (d >= -1 && d <= 1)? '#' : '.';
                ++c;
                pixels ~= pixel;
            }
            cmd.formattedRead("addx %s", v);
            x += v;
            /* x.writeln; */
        }
    }
    foreach (i; 0..6)
    {
        foreach (j; 0..40)
        {
            pixels[i * 40 + j].write;
        }
        "".writeln;
    }
}

int main()
{
    draw(readText("input.txt").splitLines);
    return 0;
}

unittest
{
    assert((8 - 9).abs <= 1);
    auto c = signals(readText("sample.txt").splitLines);
    c.writeln;
    assert(c == [420, 1140, 1800, 2940, 2880, 3960]);
    draw(readText("sample.txt").splitLines);
}
