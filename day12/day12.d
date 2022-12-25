import std.stdio;
import std.file;
import std.conv;
import std.algorithm;
import std.format;
import std.range;
import std.string;

struct Position
{
    int i;
    int j;
};

void visit(Position p, char e, string[] map, int[][] visited, ref Position[] next)
{
    if (p.i >= 0 && p.i < map.length && p.j >= 0 && p.j < map[p.i].length
            && visited[p.i][p.j] == -1 && e + 1 >= map[p.i][p.j])
    {
        next ~= p;
    }
}

int climb(string[] map)
{
    int[][] visited = new int[][](map.length, map[0].length);
    visited.each!((a) { a.fill(-1); });

    Position[] current;
    Position[] next;
    int distance = 0;
    Position target;

    foreach (i; 0 .. map.length)
    {
        auto j = map[i].indexOf('S');
        if (j != -1)
        {
            current ~= Position(i.to!int, j.to!int);
            map[i] = map[i].replace('S', 'a');
        }

        j = map[i].indexOf('E');
        if (j != -1)
        {
            target = Position(i.to!int, j.to!int);
            map[i] = map[i].replace('E', 'z');
        }
        foreach (j2; 0 .. map[i].length)
        {
            if (map[i][j2] == 'a')
            {
                current ~= Position(i.to!int, j2.to!int);
            }
        }
    }

    while (!current.empty)
    {
        foreach (p; current)
        {

            if (visited[p.i][p.j] != -1)
            {
                continue;
            }
            visited[p.i][p.j] = distance;
            if (p == target)
            {
                return distance;
            }
            auto elevation = map[p.i][p.j];
            visit(Position(p.i - 1, p.j), elevation, map, visited, next);
            visit(Position(p.i + 1, p.j), elevation, map, visited, next);
            visit(Position(p.i, p.j - 1), elevation, map, visited, next);
            visit(Position(p.i, p.j + 1), elevation, map, visited, next);
        }
        current.destroy;
        current.swap(next);
        ++distance;
    }

    return distance;
}


int main()
{
    climb(readText("input.txt").splitLines)
        .writeln;
    return 0;
}

unittest
{
    auto dist = climb(readText("sample.txt").splitLines);
    dist.writeln;
    assert(dist == 31);
}


