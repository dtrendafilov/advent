import std.stdio;
import std.file;
import std.conv;
import std.algorithm;
import std.algorithm.iteration;
import std.typecons;
import std.format;


int within(Tuple!(int, int, int, int) r)
{
    return (r[0] <= r[2] && r[3] <= r[1]) || (r[2] <= r[0] && r[1] <= r[3]);
}

int overlap(Tuple!(int, int, int, int) r)
{
    int result = (r[0] <= r[2] && r[2] <= r[1]) 
        || (r[0] <= r[3] && r[3] <= r[1])
        || (r[2] <= r[0] && r[1] <= r[3]);

    /* char[50] buff; */
    /* sformat(buff, "%s-%s %s-%s -> %s", r[0], r[1], r[2], r[3], result); */
    /* writeln(buff); */
    return result;
}


int pairs(string name)
{
    auto result = slurp!(int, int, int, int)(name, "%s-%s,%s-%s")
                .map!overlap
                .sum;
    return result;
}

int main()
{
    pairs("input.txt")
        .writeln;
    return 0;
}

unittest
{
}

