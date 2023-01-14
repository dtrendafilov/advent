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

void write_cave(char[9][] cave)
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

void place(char[9][] cave, Point[] rock, Point dir)
{
    foreach (r; rock)
    {
        cave[r.y + dir.y][r.x + dir.x] = 'X';
    }
}

bool is_floor(char[9] level)
{
    auto l = level[1..8].indexOf('X');
    auto r = level[1..8].lastIndexOf('X');
    return !(l >= 4 || r <= 2 || (r - l) >= 4);
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
                place(cave, rock, delta);
                height = max(height, delta.y + rock_height - 1);
                if (is_floor(cave[height]) && rock == shapes[$ - 1] && height > 100)
                {
                    [rock_height, delta.y, height].writeln;
                    write_cave(cave);
                    return height;
                }
                break;
            }
        } while (true);
    }


    return height;
}


int main()
{
    auto input = readText("input.txt")[0 .. $ - 1];
    input.length.writeln;

    fall(input, 2022).writeln;
    return 0;
}

unittest
{
    auto input = ">>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>";
    auto v = fall(input, 2022);
    v.writeln;
    assert(v == 3068);
}





