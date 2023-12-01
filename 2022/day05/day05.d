import std.stdio;
import std.file;
import std.conv;
import std.algorithm;
import std.algorithm.iteration;
import std.range;
import std.string;
import std.ascii;
import std.format;
import std.container.array;




auto crane(string name)
{
    auto dock = [];

    auto lines = readText(name).to!string.splitLines();
    auto input = lines.split("");
    auto initial = input[0];
    auto ops = input[1];

    auto deck = generate!(() => Array!char()).take(initial[0].length / 4 + 1).array;



    auto stacks = initial.retro.drop(1);
    foreach (l; stacks)
    {
        foreach (i, s; l.drop(1).stride(4).enumerate)
        {
            if (s.isAlphaNum) {
                deck[i].insertBack(s.to!char);
            }
        }
    }

    foreach (op; ops)
    {
        int count;
        int from;
        int to;

        formattedRead(op, "move %s from %s to %s", count, from, to);
        --from;
        --to;
        /* foreach (s; deck[from][$ - count..$].retro) */
        foreach (s; deck[from][$ - count..$])
        {
            deck[to].insertBack(s);
        }
        deck[from].length(deck[from].length - count);

    }



    return deck.map!((a) => a.back);
}

int main()
{
    crane("input.txt")
        /* .reduce!((a, b) => a ~ b) */
        .writeln;
    return 0;
}

unittest
{
}


