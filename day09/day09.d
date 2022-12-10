import std.stdio;
import std.file;
import std.conv;
import std.algorithm;
import std.algorithm.iteration;
import std.typecons;
import std.format;


struct Position
{
    int x, y;
}

alias Tuple!(char, int) Movement;

void move_with(ref Position position, Movement move)
{
    final switch (move[0])
    {
        case 'R': position.x += move[1]; break;
        case 'L': position.x -= move[1]; break;
        case 'U': position.y += move[1]; break;
        case 'D': position.y -= move[1]; break;
    }
}


int distance_to(Position lhs, Position rhs)
{
    auto dx = lhs.x - rhs.x;
    auto dy = lhs.y - rhs.y;
    return dx*dx + dy*dy;
}

bool adjacent_to(Position lhs, Position rhs)
{
    return distance_to(lhs, rhs) <= 2;
}

auto rope(Movement[] movements)
{
    bool[Position] visited;
    Position head, tail; // start at 0, 0

    visited[tail] = true;
    foreach (move; movements)
    {
        auto distance = tail.distance_to(head);
        assert(distance <= 2);

        head.move_with(move);
        if (!tail.adjacent_to(head))
        {
            int dx = head.x - tail.x;
            int dy = head.y - tail.y;
            if (distance == 2)
            {
                if (dx * dx == 1)
                {
                    tail.x = head.x;
                    tail.y += (dy > 0)? 1 : -1;
                    visited[tail] = true;
                }
                else if (dy * dy == 1)
                {
                    tail.y = head.y;
                    tail.x += (dx > 0)? 1 : -1;
                    visited[tail] = true;
                }
            }
            dx = head.x - tail.x;
            dy = head.y - tail.y;
            if (dx != 0)
            {
                int sx = (dx > 0)? 1 : -1;
                while (tail.x + sx != head.x)
                {
                    tail.x += sx;
                    visited[tail] = true;
                }
            }
            if (dy != 0)
            {
                int sy = (dy > 0)? 1 : -1;
                while (tail.y + sy != head.y)
                {
                    tail.y += sy;
                    visited[tail] = true;
                }
            }
        }
    }
    visited.writeln;

    return visited.length;
}


auto bridge(string name)
{
    auto result = slurp!(char, int)(name, "%s %s")
                .rope;
    return result;
}

int main()
{
    bridge("input.txt")
        .writeln;
    return 0;
}

unittest
{
    bridge("sample.txt").writeln;
    assert(bridge("sample.txt") == 13);
}


