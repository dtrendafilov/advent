import std.stdio;
import std.file;
import std.range;
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

void step(ref Position position, char move)
{
    final switch (move)
    {
        case 'R': ++position.x; break;
        case 'L': --position.x; break;
        case 'U': ++position.y; break;
        case 'D': --position.y; break;
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

template move_next_to(void delegate(Position) update)
{
    auto move_next_to(ref Position knot, Position lead)
    /* in */
    /* { */
    /*     knot.write; */
    /*     " -> ".write; */
    /*     lead.write; */
    /*     " -> ".write; */
    /* } */
    /* out */
    /* { */
    /*     knot.writeln; */
    /* } */
    /* do */
    {
        if (knot.adjacent_to(lead))
        {
            return false;
        }

        int dx = lead.x - knot.x;
        int dy = lead.y - knot.y;
        int sx = (dx > 0)? 1 : ((dx < 0)? -1 : 0);
        int sy = (dy > 0)? 1 : ((dy < 0)? -1 : 0);

        while (dx != 0 && dy != 0 && (dx * dx + dy * dy > 2))
        {
            knot.x += sx;
            knot.y += sy;
            update(knot);
            dx -= sx;
            dy -= sy;
        }
        while (knot.x != lead.x && knot.x + sx != lead.x)
        {
            knot.x += sx;
            update(knot);
        }
        while (knot.y != lead.y && knot.y + sy != lead.y)
        {
            knot.y += sy;
            update(knot);
        }
        return true;
    }
}

auto rope(Movement[] movements)
{
    bool[Position] visited;
    Position head, tail; // start at 0, 0

    visited[tail] = true;
    foreach (move; movements)
    {
        auto distance = tail.distance_to(head);
        assert(tail.adjacent_to(head));

        head.move_with(move);
        move_next_to!((a) {visited[a] = true;})(tail, head);
    }

    return visited;
}

auto rope10(Movement[] movements)
{
    bool[Position] visited;
    Position[] rope = new Position[](10); // start at 0, 0

    visited[rope[9]] = true;
    foreach (move; movements)
    {
        /* auto distance = tail.distance_to(head); */
        /* assert(tail.adjacent_to(head)); */

        while(move[1]--)
        {
            rope[0].step(move[0]);
            foreach (int index; 1 .. 9)
            {
                rope[index].move_next_to!((a) { })(rope[index - 1]);
            }
            rope[9].move_next_to!((a) { visited[a] = true; })(rope[8]);
        }
    }

    return visited;
}


auto bridge(string name)
{
    auto result = slurp!(char, int)(name, "%s %s")
                .rope10;
    return result;
}

int main()
{
    bridge("input.txt")
        .length
        .writeln;
    return 0;
}

unittest
{
    /* assert(bridge("sample.txt") == 13); */

    void dump_state(bool[Position] visited)
    {
        auto min_x = visited.byKey.map!((a) => a.x).minElement;
        auto max_x = visited.byKey.map!((a) => a.x).maxElement;

        auto min_y = visited.byKey.map!((a) => a.y).minElement;
        auto max_y = visited.byKey.map!((a) => a.y).maxElement;

        [min_y, max_y, min_x, max_x].writeln;

        foreach (y; iota(max_y, min_y - 1, -1))
        {
            foreach (x; min_x .. max_x + 1)
            {
                if (Position(x, y) in visited)
                {
                    '#'.write;
                }
                else
                {
                    '.'.write;
                }
            }
            "".writeln;
        }
    }

    dump_state(bridge("sample.txt"));
    assert(bridge("sample.txt").length == 1);
    /* assert(bridge("sample.txt").length == 13); */

    auto k2 = Position(3, 0);
    k2.move_next_to!((a) {})(Position(5, 7));
    k2.writeln;
    assert(k2 == Position(5, 6));


    auto s2 = bridge("sample2.txt");
    dump_state(s2);
    assert(s2.length == 36);
}


