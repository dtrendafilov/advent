import std.algorithm;
import std.conv;
import std.file;
import std.format;
import std.math;
import std.range;
import std.stdio;
import std.string;
import std.typecons;
import std.functional;

struct Inventory
{
    int ore;
    int clay;
    int obsidian;
    int geode;

    ref Inventory opOpAssign(string op)(Inventory rhs) if (op == "+") {
        ore += rhs.ore;
        clay += rhs.clay;
        obsidian += rhs.obsidian;
        geode += rhs.geode;
        return this;
    }
    ref Inventory opOpAssign(string op)(Inventory rhs) if (op == "-") {
        ore -= rhs.ore;
        clay -= rhs.clay;
        obsidian -= rhs.obsidian;
        geode -= rhs.geode;
        return this;
    }

    Inventory opBinary(string op)(Inventory rhs)
    {
        Inventory r = this;
        mixin("r " ~ op ~ "= rhs;");
        return r;
    }

    bool canAfford(Inventory rhs) const
    {
        return ore >= rhs.ore && clay >= rhs.clay 
            && obsidian >= rhs.obsidian && geode >= rhs.geode;
    }

    ref int opIndex(size_t index)
    {
        final switch (index)
        {
            case 0: return ore;
            case 1: return clay;
            case 2: return obsidian;
            case 3: return geode;
        }
    }
}

alias Inventory[4] Blueprint;

Blueprint read_blueprint(string input)
{
    Blueprint blueprint;
    int index;
    input.formattedRead("Blueprint %s: Each ore robot costs %s ore. Each clay robot costs %s ore. Each obsidian robot costs %s ore and %s clay. Each geode robot costs %s ore and %s obsidian.",
            index,
            blueprint[0].ore,
            blueprint[1].ore,
            blueprint[2].ore, blueprint[2].clay,
            blueprint[3].ore, blueprint[3].obsidian);
    return blueprint;
}

Blueprint[] load_factory(string input)
{
    return readText(input).splitLines
            .map!read_blueprint
            .array;
}

const int memo_size = 128 * 1024 * 1024;

int score_d(Inventory score, Inventory production, int factory_output, Blueprint blueprint, int time)
{
    if (time <= 0)
    {
        /* score.writeln; */
        return score.geode;
    }

    score += production;
    if (factory_output != -1)
    {
        ++production[factory_output];
    }
    if (score.canAfford(blueprint[3]))
    {
        return memoize!score_d(score - blueprint[3], production, 3, blueprint,
                time - 1);
    }
    /*
    if (score.canAfford(blueprint[2]))
    {
        return memoize!score_d(score - blueprint[2], production, 2, blueprint,
                time - 1);
    }
    int result = memoize!score_d(score, production, -1, blueprint, time - 1);
    foreach (action; 0..2)
    */
    int result = memoize!score_d(score, production, -1, blueprint, time - 1);
    foreach (action; 0..3)
    {
        auto cost = blueprint[action];
        if (score.canAfford(cost))
        {
            result = max(result, memoize!score_d(score - cost, production, action,
                        blueprint, time - 1));
        }
    }

    return result;
}

alias score_m = memoize!score_d;

int score_blueprint(Blueprint blueprint)
{
    Inventory score;
    auto production = Inventory(1, 0, 0, 0);
    auto s = score_m(score, production, -1, blueprint, 24);
    s.writeln;
    return s;
}

int score_blueprint_2(Blueprint blueprint)
{
    Inventory score;
    auto production = Inventory(1, 0, 0, 0);
    auto s = score_m(score, production, -1, blueprint, 22);
    s.writeln;
    return s;
}

ulong score_factory(Blueprint[] factory, int start = 1)
{
    return factory.map!score_blueprint
        .enumerate
        .map!((index_score) => (index_score[0] + start) * index_score[1])
        .sum;
}

ulong score_factory2(Blueprint[] factory, int start = 1)
{
    return factory.take(3)
        .map!score_blueprint_2
        .fold!((a, b) => a * b);
}

int main1_1()
{
    auto s = load_factory("input.txt").score_factory;
    s.writeln;
    return 0;
}

int main()
{
    auto s = load_factory("input.txt").score_factory2;
    s.writeln;
    return 0;
}

int main1_2()
{
    auto s = load_factory("input2.txt").score_factory(16);
    s.writeln;
    return 0;
}

unittest
{
    {
        auto l = Inventory(0, 1, 0, 0);
        auto r = Inventory(1, 1, 0, 0);
        assert(!l.canAfford(r));
        assert(r.canAfford(l));
    }
    auto s = load_factory("sample.txt").score_factory2;
    s.writeln;
    assert(s == 33);
}

