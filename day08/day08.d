import std.stdio;
import std.file;
import std.conv;
import std.algorithm;
import std.algorithm.iteration;
import std.range;
import std.format;
import std.string;


void mark_visible_cols(int dx, int dy)(string[] forest, bool[][] visible)
{
    int bx = (dx > 0)? 0 : forest[0].length.to!int - 1;
    int ex = (dx > 0)? forest[0].length.to!int : -1;

    int by = (dy > 0)? 0 : forest.length.to!int - 1;
    int ey = (dy > 0)? forest.length.to!int : -1;
    for (auto x = bx; x != ex; x += dx)
    {
        int start = '0' - 1;
        for (auto y = by; y != ey; y += dy)
        {
            if (forest[y][x] > start)
            {
                visible[y][x] = true;
                start = forest[y][x];
            }
        }
    }
}

void mark_visible_rows(int dx, int dy)(string[] forest, bool[][] visible)
{
    int bx = (dx > 0)? 0 : forest[0].length.to!int - 1;
    int ex = (dx > 0)? forest[0].length.to!int : -1;

    int by = (dy > 0)? 0 : forest.length.to!int - 1;
    int ey = (dy > 0)? forest.length.to!int : -1;
    for (auto y = by; y != ey; y += dy)
    {
        int start = '0' - 1;
        for (auto x = bx; x != ex; x += dx)
        {
            if (forest[y][x] > start)
            {
                visible[y][x] = true;
                start = forest[y][x];
            }
        }
    }
}


auto count_visible(string[] forest)
{
    bool[][] visible = new bool[][](forest.length, forest[0].length);
    mark_visible_rows!(1, 1)(forest, visible);
    mark_visible_rows!(-1, 1)(forest, visible);
    mark_visible_cols!(1, 1)(forest, visible);
    mark_visible_cols!(1, -1)(forest, visible);
    int count = 0;
    foreach (r; visible)
    {
        foreach (t; r)
        {
            if (t)
            {
                ++count;
            }
        }
    }
    return count;
}

auto scenic_score(string[] forest, int x, int y)
{
    int top = y - 1;
    while (top > 0 && forest[top][x] < forest[y][x]) --top;
    int bottom = y + 1;
    while (bottom < forest.length - 1 && forest[bottom][x] < forest[y][x]) ++bottom;

    int left = x - 1;
    while (left > 0 && forest[y][left] < forest[y][x]) --left;
    int right = x + 1;
    while (right < forest[0].length - 1 && forest[y][right] < forest[y][x]) ++right;

    /* [x, y, top, bottom, left, right].writeln; */
    return (y - top) * (bottom - y) * (x - left) * (right - x);
}

auto scenic(string[] forest)
{
    int score = 0;
    foreach (x; 1 .. forest.length - 1)
    {
        foreach (y; 1 .. forest[0].length - 1)
        {
            auto xy_score = scenic_score(forest, x.to!int, y.to!int);
            /* [x, y, xy_score].writeln; */
            score = max(xy_score, score);
        }
    }
    return score;
}

int main()
{
    scenic(readText("input.txt").splitLines)
        .writeln;
    return 0;
}

unittest
{
    count_visible(readText("sample.txt").splitLines).writeln;
    assert(count_visible(readText("sample.txt").splitLines) == 21);

    scenic(readText("sample.txt").splitLines).writeln;
    assert(scenic(readText("sample.txt").splitLines) == 8);
}
