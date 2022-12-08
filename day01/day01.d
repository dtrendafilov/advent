import std.stdio, std.array, std.algorithm;
import std.conv;
import std.algorithm.comparison;
import std.algorithm.iteration;

auto sum_tuple(T)(T tuple)
{
    return tuple[1].map!((a) => a.to!int)
        .sum;
}

void main()
{
    int[] calories = stdin.byLineCopy
        .array
        .chunkBy!((a) => !a.empty)
        .filter!((a) => a[0])
        .map!(sum_tuple)
        .array;
    calories.topN!"a > b"(3);
    auto top3 = calories[0..3];
    top3.sum.writeln;
}
