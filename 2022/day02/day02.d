import std.stdio;
import std.conv;
import std.algorithm;
import std.algorithm.iteration;
import std.file;

enum RPS
{
    Rock,
    Paper,
    Scissors,
}

int score(RPS other, RPS own)
{
    if (other == own) {
        return 1;
    }
    return 2 * (own - other == 1 || (other == RPS.Scissors && own == RPS.Rock));
}

int score(char other, char own)
{
    auto e = (other - 'A').to!RPS;
    auto o = (own - 'X').to!RPS;
    return (1 + o.to!int) + 3 * score(e, o);
}

int play(char other, char result)
{
    auto o = other - 'A';
    auto r = (result - 'X') - 1;
    auto own = (3 + o + r) % 3;
    return (1 + own.to!int) + 3 * (result - 'X');
}

unittest
{
    assert((-1 % 3) == -1);
    assert(play('A', 'Y') == 4);
    assert(play('B', 'X') == 1);
    assert(play('C', 'Z') == 7);
}

int main()
{
    slurp!(char, char)("input.txt", "%s %s")
        .map!((a) => play(a[0], a[1]))
        .sum
        .writeln;
    return 0;
}
