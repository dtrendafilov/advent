import std.algorithm;
import std.conv;
import std.file;
import std.format;
import std.range;
import std.regex;
import std.stdio;
import std.string;

struct Point
{
    int x;
    ulong y;
}

void write_cave(const char[9][] cave)
{
    foreach (line; cave.retro)
    {
        line.writeln;
    }
}

bool can_move(char[9][] cave, Point[] rock, Point dir)
{
    foreach (r; rock)
    {
        if (cave[r.y + dir.y][r.x + dir.x] != '.')
        {
            return false;
        }
    }
    return true;
}

void place(char[9][] cave, Point[] rock, Point dir, char marker)
{
    foreach (r; rock)
    {
        cave[r.y + dir.y][r.x + dir.x] = marker;
    }
}

bool is_floor(char[9] level)
{
    auto l = level[1..8].indexOf('X');
    auto r = level[1..8].lastIndexOf('X');
    return !(l >= 4 || r <= 2 || (r - l) >= 4);
}

int count_filled(const char[9][] pattern)
{
    auto filled = 4;
    foreach (row; pattern)
    {
        filled += 7 - row[1 .. $ - 1].count('.');
    }
    return filled;
}

int count_rocks(const char[9][] pattern)
{
    auto filled = count_filled(pattern);
    if (filled % 22 != 0)
    {
        /* writeln("Filled:",filled % 22); */
        /* write_cave(pattern); */
    }
    return (filled / 22) * 5;
}

void analyze(char[9][] cave)
{
    /* write_cave(cave); */
    int[char[9][]] patterns;
    foreach (pattern; cave[1..$].splitter(cave[0]))
    {
        patterns.update(pattern, () => 1, (ref int c) { ++c; });
    }
    alias Pattern = const char[9][];
    Pattern[] unique;
    Pattern[] repeat;
    auto total = 0;
    foreach (pattern, c; patterns)
    {
        total += c * (pattern.length + 1);
        writeln(c, ',', pattern.length + 1, ',', count_rocks(pattern));
        if (c == 1)
        {
            unique ~= pattern;
        }
        else
        {
            repeat ~= pattern;
        }
    }
    auto count_u = unique.map!count_filled.sum;
    writeln("unique: ", count_u, ' ', count_u % 22, ' ', (count_u / 22) * 5);
    auto count_r = repeat.map!count_filled.sum;
    writeln("repeat: ", count_r, ' ', count_r % 22, ' ', (count_r / 22) * 5);
    writeln("total:", total);
}


ulong fall(string input, ulong n)
{
    auto shapes = [
        [ Point(0, 0), Point(1, 0), Point(2, 0), Point(3, 0) ],
        [ Point(1, 0), Point(0, 1), Point(1, 1), Point(2, 1), Point(1, 2) ],
        [ Point(0, 0), Point(1, 0), Point(2, 0), Point(2, 1), Point(2, 2) ],
        [ Point(0, 0), Point(0, 1), Point(0, 2), Point(0, 3) ],
        [ Point(0, 0), Point(1, 0), Point(0, 1), Point(1, 1) ],
    ];

    auto rocks = shapes.cycle;
    auto markers = "ABCDE".cycle;

    auto heights = [1, 3, 3, 4, 2].cycle;

    char[9][] cave;
    ulong height = 0;

    cave ~= ['#', '#', '#', '#', '#', '#', '#', '#', '#'];
    cave ~= ['#', '.', '.', '.', '.', '.', '.', '.', '#'];
    cave ~= ['#', '.', '.', '.', '.', '.', '.', '.', '#'];
    cave ~= ['#', '.', '.', '.', '.', '.', '.', '.', '#'];

    auto jets = input.map!((j) => (j == '<')? -1 : 1).array.cycle;
    

    foreach (r; 0..n)
    {
        int rock_height = heights.front.to!int;
        foreach (h; cave.length .. height + 4 + rock_height)
        {
            cave ~= ['#', '.', '.', '.', '.', '.', '.', '.', '#'];
        }
        heights.popFront;


        auto rock = rocks.front;
        rocks.popFront;

        auto delta = Point(3, height + 4);

        do {
            auto jet = jets.front;
            jets.popFront;
            if (can_move(cave, rock, Point(delta.x + jet, delta.y)))
            {
                delta.x += jet;
            }
            if (can_move(cave, rock, Point(delta.x, delta.y - 1)))
            {
                delta.y -= 1;
            }
            else
            {
                place(cave, rock, delta, markers.front.to!char);
                markers.popFront;
                height = max(height, delta.y + rock_height - 1);
                /* if (is_floor(cave[height]) && rock == shapes[$ - 1] && height > 100) */
                /* { */
                /*     [rock_height, delta.y, height].writeln; */
                /*     write_cave(cave); */
                /*     return height; */
                /* } */
                break;
            }
        } while (true);
    }
    
    /* analyze(cave[1 .. height + 1]); */

    return height;
}

ulong fall2(string input)
{
    ulong[1725] reminders;
    reminders[0] = 0;
    foreach (n; 1..1725)
    {
        reminders[n] = fall(input, 190 + n) - 300;
    }
    /* auto v = fall(input, 2022); */
    auto c = 1000000000000 - 190;
    auto v2 = 300 + 2659 * (c / 1725) + reminders[c % 1725];
    /* writeln(v, ' ', v2); */
    return v2;
}


int main()
{
    auto input = readText("input.txt")[0 .. $ - 1];

    /* fall(input, 190).writeln; */
    /* fall(input, 190 + 1725).writeln; */
    /* fall(input, 10000015 + 1725 * (3 - 5800)).writeln; */
    fall2(input).writeln;
    return 0;
}

unittest
{
    auto input = ">>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>";
    /* auto input = readText("input.txt")[0 .. $ - 1]; */
    ulong[35] reminders;
    reminders[0] = 0;
    foreach (n; 1..35)
    {
        reminders[n] = fall(input, 15 + n) - 25;
    }
    auto v = 0;//fall(input, 1000);
    auto c = 1000000000000 - 15;
    auto v2 = 25 + 53 * (c / 35) + reminders[c % 35];
    writeln(v, ' ', v2);

    assert(v == 3068);
}

