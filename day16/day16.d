import std.algorithm;
import std.conv;
import std.file;
import std.format;
import std.math;
import std.range;
import std.regex;
import std.stdio;
import std.string;
import std.typecons;

class Valve
{
    int flow;
    bool open = false;
    string[] links;
    int mask;
}

alias Valve[string] Valves;

auto read_valve(string input)
{
    auto m = input.matchFirst(`Valve (.+) has flow rate=(\d+); tunnels? leads? to valves? (.+)`);
    assert(m);
    string name = m[1];
    Valve valve = new Valve;
    valve.flow = m[2].to!int;
    valve.links = m[3].splitter(", ").array;
    return Tuple!(string, Valve)(name, valve);
}


Valves load_valves(string input)
{
    auto valves = readText(input).splitLines
            .map!read_valve
            .assocArray
            ;
    assert(valves.length < 64);
    int i = 0;
    foreach (v; valves)
    {
        v.mask = (1 << i);
        ++i;
    }
    return valves;
}


auto open_valves(Valves valves)
{
    int[string][] flow = new int[string][](31);
    write(format("%2s:", 1));
    foreach (k, v; valves)
    {
        flow[0][k] = 0;
        flow[1][k] = 0;
        write(format("%5s", k));

    }
    "".writeln;
    foreach (t; 2..31)
    {
        write(format("%2s:", t));
        foreach (k, v; valves)
        {
            auto no_open = v.links.map!((l) => flow[t - 1][l]).maxElement;
            if (v.flow)
            {
                auto open_now = v.flow + flow[t - 1][k];
                flow[t][k] = max(open_now, no_open);
            }
            else
            {
                flow[t][k] = no_open;
            }
            write(format("%5s", flow[t][k]));
        }
        "".writeln;
    }
    return flow[30]["AA"];
}


auto open_valves_top(Valves valves, int limit)
{
    struct State
    {
        string Start;
        ulong Open;
        int Time;
    }
    int[State] memory;


    auto open_valve(string n, ulong open_state, int open, int t)
    {
        if (t <= 0)
        {
            return 0;
        }
        
        auto s = State(n, open_state, t);
        {
            if (s in memory)
            {
                return memory[s];
            }

        }

        auto v = valves[n];

        auto no_open = v.links.map!((l) => open_valve(l, open_state, open, t - 1)).maxElement;
        if (v.flow && !(v.mask & open_state))
        {
            open_state |= v.mask;
            auto do_open = open_valve(n, open_state, open + v.flow, t - 1);
            if (do_open > no_open)
            {
                memory[s] = open + do_open;
                return open + do_open;
            }
        }

        memory[s] = no_open + open;
        return no_open + open;
    }
    auto r = open_valve("AA", 0, 0, limit);
    return r;
}

auto open_valves_2(Valves valves, int limit)
{
    struct State2
    {
        string Me;
        string Elephant;
        ulong Open;
        int Time;
    }
    int[State2] memory;


    auto open_valve(string me, string elephant, ulong open_state, int open, int t)
    {
        if (t <= 0)
        {
            return 0;
        }
        
        auto s = State2(me, elephant, open_state, t);
        {
            if (s in memory)
            {
                return memory[s];
            }
        }
        auto s2 = State2(elephant, me, open_state, t);
        {
            if (s2 in memory)
            {
                return memory[s2];
            }
        }

        auto v_me = valves[me];
        auto v_el = valves[elephant];

        auto moves = cartesianProduct(v_me.links, v_el.links);

        auto no_open = moves.map!((m) => open_valve(m[0], m[1], open_state, open, t - 1)).maxElement;
        auto open_me = 0;
        auto open_el = 0;
        auto open_both = 0;
        if (v_me.flow && !(v_me.mask & open_state))
        {
            open_state |= v_me.mask;
            open_me = v_el.links.map!((el) => open_valve(me, el, open_state,
                        open + v_me.flow, t - 1)).maxElement;
            open_state ^= v_me.mask;
        }
        if (me != elephant && v_el.flow && !(v_el.mask & open_state))
        {
            open_state |= v_el.mask;
            open_el = v_me.links.map!((m) => open_valve(m, elephant, open_state,
                        open + v_el.flow, t - 1)).maxElement;
            open_state ^= v_el.mask;
        }
        if (me != elephant && v_me.flow && !(v_me.mask & open_state) && v_el.flow && !(v_el.mask & open_state))
        {
            open_state |= v_el.mask | v_me.mask;
            open_both = open_valve(me, elephant, open_state, open + v_el.flow +
                    v_me.flow, t - 1);
        }

        auto released = open + [no_open, open_me, open_el, open_both].maxElement;
        memory[s] = released;
        if (memory.length % 1000000 == 0)
        {
            memory.length.writeln;
        }
        return released;
    }
    auto r = open_valve("AA", "AA", 0, 0, limit);
    memory.length.writeln;
    return r;
}


int main()
{
    auto valves = load_valves("input.txt");
    auto flow = valves.open_valves_2(26);
    flow.writeln;
    return 0;
}

unittest
{
    auto valves = load_valves("sample.txt");
    /* foreach (t; 2..12) */
    /* { */
    /*     auto flow = valves.open_valves_top(t); */
    /*     format("%s -> %s", t, flow).writeln; */
    /* } */
    auto t = 26;
    auto flow = valves.open_valves_2(t);
    format("%s -> %s", t, flow).writeln;
    assert(flow == 1651);
}




