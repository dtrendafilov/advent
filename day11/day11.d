import std.stdio;
import std.file;
import std.conv;
import std.algorithm;
import std.format;
import std.range;
import std.regex;
import std.string;
import std.math;


enum Operation
{
    Add,
    Mul,
    Square,
}

class Monkey
{
    ulong[] items;
    int[2] next;
    int test;
    ulong arg;
    ulong inspections;
    Operation op;

    int decide(ulong item)
    {
        return next[item % test == 0];
    }

    ulong inspect(ulong item)
    in {}
    out (result)
    {
        assert(result > item);
    }
    do
    {
        ++inspections;
        final switch (op)
        {
            case Operation.Add: return item + arg;
            case Operation.Mul: return item * arg;
            case Operation.Square: return item * item;
        }
    }

    int inspect_decide(ulong item)
    {
        ++inspections;
        final switch (op)
        {
            case Operation.Add: return next[(item + arg) % test == 0];
            case Operation.Mul:
                        return next[item % test == 0 || arg % test == 0];
            case Operation.Square: return next[item % test == 0];
        }
    }

    override string toString()
    {
        return "%s %s %s %s %s".format(op, arg, test, next, items);
    }


}

Monkey read_monkey(string[] input)
{
    auto monkey = new Monkey;
    static immutable items_start = "  Starting items: ".length;
    monkey.items = input[1][items_start..$]
                    .splitter(regex(", "))
                    .map!((a) => a.to!ulong)
                    .array;
    
    static immutable op_index = "  Operation: new = old ".length;
    if (input[2][op_index] == '+')
    {
        monkey.op = Operation.Add;
    }
    else
    {
        assert(input[2][op_index] == '*');
        if (input[2][op_index + 2] == 'o')
        {
            monkey.op = Operation.Square;
        }
        else
        {
            monkey.op = Operation.Mul;
        }

    }
    if (monkey.op == Operation.Add || monkey.op == Operation.Mul)
    {
        monkey.arg = input[2][op_index + 2 .. $].to!ulong;
    }

    static immutable test_start = "  Test: divisible by ".length;
    monkey.test = input[3][test_start .. $].to!int;

    static immutable true_s = "    If true: throw to monkey ".length;
    auto true_n = input[4][true_s .. $].to!int;
    static immutable false_s = "    If false: throw to monkey ".length;
    auto false_n = input[5][false_s .. $].to!int;
    monkey.next = [false_n, true_n];

    return monkey;
}


auto keep_away(Input)(Input input)
{
    Monkey[] monkeys = input.chunks(7).map!array.map!read_monkey.array;

    ulong modulo = monkeys.map!((a) => a.test).fold!((a, b) => a * b);

    foreach (round; 0 .. 10000)
    {
        foreach (monkey; monkeys)
        {
            foreach (item; monkey.items)
            {
                /* int level = monkey.inspect(item) / 3; */
                ulong level = monkey.inspect(item) % modulo;
                int throw_to = monkey.decide(level);
                monkeys[throw_to].items ~= level;
                /* int throw_to = monkey.inspect_decide(item); */
                /* monkeys[throw_to].items ~= item; */
            }
            monkey.items.destroy();
        }
        /* monkeys.map!((a) => a.inspections).writeln; */
    }

    ulong[] inspections = monkeys.map!((a) => a.inspections).array;
    inspections.sort;
    return inspections[$ - 1] * inspections[$ - 2];
}


int main()
{
    keep_away(readText("input.txt").lineSplitter)
        .writeln;
    return 0;
}

unittest
{
    keep_away(readText("sample.txt").lineSplitter)
        .writeln;
}

