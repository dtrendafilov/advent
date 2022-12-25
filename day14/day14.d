import std.stdio;
import std.file;
import std.conv;
import std.algorithm;
import std.algorithm.iteration;
import std.format;
import std.range;
import std.string;


Point read_point(string input)
{
    Point result;
    input.formattedRead("%s,%s", result.x, result.y);
    return result;
}

Point[] rockface(T)(T input)
{
    return input.map!read_point.array;
}

Point[][] read_input(string input)
{
    return readText(input).splitLines
        .map!((a) => a.splitter(" -> "))
        .map!((a) => rockface(a))
        .array;
}

enum Tile
{
    Abyss,
    Air,
    Rock,
    Sand,
}

struct Point
{
    int x;
    int y;
}

Tile[][] cave(Point[][] input)
{
    auto minimum = Point(500, 0);
    Point maximum;
    foreach (face; input)
    {
        foreach (point; face)
        {
            minimum.x = min(minimum.x, point.x);
            minimum.y = min(minimum.y, point.y);
            maximum.x = max(maximum.x, point.x);
            maximum.y = max(maximum.y, point.y);
        }
    }
    assert(minimum.x >= 0);
    assert(minimum.y >= 0);
    minimum.x = 0;
    maximum.x = 10001;
    auto floor = maximum.y + 2;
    auto cave_map = new Tile[][](maximum.x + 3, floor + 2);
    
    foreach (x; minimum.x .. maximum.x + 1) 
    {
        foreach (y; 0 .. maximum.y + 2) 
        {
            cave_map[x+1][y] = Tile.Air;
        }
        cave_map[x+1][floor] = Tile.Rock;
    }

    foreach (face; input)
    {
        auto current = face[0];
        
        foreach (point; face[1 .. $])
        {
            minimum.x = min(current.x, point.x);
            minimum.y = min(current.y, point.y);
            maximum.x = max(current.x, point.x);
            maximum.y = max(current.y, point.y);


            foreach (x; minimum.x .. maximum.x + 1)
            {
                foreach (y; minimum.y .. maximum.y + 1)
                {
                    cave_map[x+1][y] = Tile.Rock;
                }
            }

            current = point;
        }
    }


    return cave_map;
}

int sand(Tile[][] cave_map)
{
    int sand = 0;
    while (true)
    {
        auto s = Point(501, 0);
        if (cave_map[s.x][s.y] == Tile.Sand)
        {
            return sand;
        }
        while (true)
        {
            if (cave_map[s.x][s.y] == Tile.Abyss)
            {
                return sand;
            }
            else if (cave_map[s.x][s.y + 1] <= Tile.Air)
            {
                ++s.y;
                continue;
            }
            else if (cave_map[s.x - 1][s.y + 1] <= Tile.Air)
            {
                --s.x;
                ++s.y;
            }

            else if (cave_map[s.x + 1][s.y + 1] <= Tile.Air)
            {
                ++s.x;
                ++s.y;
            }
            else
            {
                assert(cave_map[s.x][s.y] == Tile.Air);
                cave_map[s.x][s.y] = Tile.Sand;
                ++sand;
                break;
            }
        }
    }
    return sand;
}

int main()
{
    read_input("input.txt").cave.sand.writeln;

    return 0;
}

void dump_cave(Tile[][] c)
{
    foreach (y; 0..11)
    {
        foreach (x; 492..506)
        {
            final switch(c[x][y])
            {
                case Tile.Air: '.'.write; break;
                case Tile.Rock: '#'.write; break;
                case Tile.Sand: 'o'.write; break;
                case Tile.Abyss: '~'.write; break;
            }
        }
        "".writeln;
    }
}

unittest
{
    auto c = read_input("sample.txt").cave;
    c.dump_cave;

    c.sand.writeln;
}




