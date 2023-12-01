import std.algorithm;
import std.conv;
import std.file;
import std.format;
import std.range;
import std.regex;
import std.stdio;
import std.string;

struct Position
{
    int x;
    int y;
    int z;
}

const int MAX=32;

Position[] load_cubes(string file)
{
    auto input = readText(file);
    Position[] result;
    foreach (line; input.splitLines)
    {
        Position p;
        line.formattedRead("%s,%s,%s", p.x, p.y, p.z);
        p.x += 2;
        p.y += 2;
        p.z += 2;
        assert(p.x < MAX);
        assert(p.y < MAX);
        assert(p.z < MAX);
        result ~= p;
    }
    return result;
}

int surface_area(Position[] positions)
{
    int[MAX][MAX][] space = new int[MAX][MAX][MAX];

    foreach (cell; positions)
    {
        space[cell.x][cell.y][cell.z] = 1;
    }

    int area = 0;
    foreach (cell; positions)
    {
        area += 6
                - space[cell.x - 1][cell.y][cell.z]
                - space[cell.x + 1][cell.y][cell.z]
                - space[cell.x][cell.y - 1][cell.z]
                - space[cell.x][cell.y + 1][cell.z]
                - space[cell.x][cell.y][cell.z - 1]
                - space[cell.x][cell.y][cell.z + 1]
            ;
    }
    return area;
}

int surface_area_out(Position[] positions)
{
    int[MAX][MAX][] space = new int[MAX][MAX][MAX];

    foreach (cell; positions)
    {
        space[cell.x][cell.y][cell.z] = 1;
    }
    writeln("Filled");

    space[0][0][0] = 2;
    Position[] current = [ Position(0, 0, 0) ];
    Position[] next_lev;
    int outside = 0;
    while (current.length)
    {
        outside += current.length;
        foreach (cell; current)
        {
            if (cell.x > 0 && !space[cell.x - 1][cell.y][cell.z])
            {
                space[cell.x - 1][cell.y][cell.z] = 2;
                next_lev ~= Position(cell.x - 1, cell.y, cell.z);
            }
            if (cell.x < MAX - 1 && !space[cell.x + 1][cell.y][cell.z])
            {
                space[cell.x + 1][cell.y][cell.z] = 2;
                next_lev ~= Position(cell.x + 1, cell.y, cell.z);
            }

            if (cell.y > 0 && !space[cell.x][cell.y - 1][cell.z])
            {
                space[cell.x][cell.y - 1][cell.z] = 2;
                next_lev ~= Position(cell.x, cell.y - 1, cell.z);
            }
            if (cell.y < MAX - 1 && !space[cell.x][cell.y + 1][cell.z])
            {
                space[cell.x][cell.y + 1][cell.z] = 2;
                next_lev ~= Position(cell.x, cell.y + 1, cell.z);
            }

            if (cell.z > 0 && !space[cell.x][cell.y][cell.z - 1])
            {
                space[cell.x][cell.y][cell.z - 1] = 2;
                next_lev ~= Position(cell.x, cell.y, cell.z - 1);
            }
            if (cell.z < MAX - 1 && !space[cell.x][cell.y][cell.z + 1])
            {
                space[cell.x][cell.y][cell.z + 1] = 2;
                next_lev ~= Position(cell.x, cell.y, cell.z + 1);
            }
        }
        current = next_lev;
        next_lev = [];
    }
    positions[$-1].writeln;
    outside.writeln;
    writeln("Flow");

    int area = 0;
    int inside = 0;
    foreach (x; 1..MAX - 1)
    {
        foreach (y; 1..MAX - 1)
        {
            foreach (z; 1..MAX - 1)
            {
                if (space[x][y][z] == 2)
                {
                    area = area
                        + (space[x - 1][y][z] == 1).to!int
                        + (space[x + 1][y][z] == 1).to!int
                        + (space[x][y - 1][z] == 1).to!int
                        + (space[x][y + 1][z] == 1).to!int
                        + (space[x][y][z - 1] == 1).to!int
                        + (space[x][y][z + 1] == 1).to!int
                        ;
                }
                else if (!space[x][y][z])
                {
                    inside = inside
                        + (space[x - 1][y][z] == 1).to!int
                        + (space[x + 1][y][z] == 1).to!int
                        + (space[x][y - 1][z] == 1).to!int
                        + (space[x][y + 1][z] == 1).to!int
                        + (space[x][y][z - 1] == 1).to!int
                        + (space[x][y][z + 1] == 1).to!int
                        ;
                }
            }
        }
    }
    writeln("in ", 4364 - inside);
    return area;
}


int main()
{
    auto cubes = load_cubes("input.txt");
    auto v = surface_area_out(cubes);
    v.writeln;
    return 0;
}

unittest
{
    auto cubes = load_cubes("sample.txt");
    auto v = surface_area_out(cubes);
    v.writeln;
    assert(v == 58);
}


