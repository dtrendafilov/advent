import std.stdio;
import std.file;
import std.conv;
import std.algorithm;
import std.algorithm.iteration;
import std.range;
import std.format;
import std.string;


class Entry
{
    int size = 0;
    Entry[string] children;
    Entry parent;
}

int du(Entry e)
{
    foreach(child; e.children)
    {
        e.size += du(child);
    }
    return e.size;
}

int candidates(Entry e)
{
    int c = (e.size <= 100000)? e.size : 0;
    return c + e.children.byValue.map!candidates.sum;
}

int free_up(Entry e, int size)
{
    int c = (e.size < size)? 700000000 : e.size;
    return chain([c], e.children.byValue.map!((a) => free_up(a, size))).minElement;
}

auto find_space(string[] input)
{
    Entry root = new Entry();
    root.parent = root;

    Entry current = root;
    foreach (cmd; input)
    {
        if (cmd.startsWith("$ cd"))
        {
            auto d = cmd[5..$];
            if (d == "..")
            {
                current = current.parent;
            }
            else if (d != "/")
            {
                current = current.children[d];
            }
        }
        else if (cmd.startsWith("$ ls"))
        {
        }
        else if (cmd.startsWith("dir "))
        {
            auto d = cmd[4..$];
            auto child = new Entry();
            current.children[d] = child;
            child.parent = current;
        }
        else
        {
            int size;
            string name;
            formattedRead(cmd, "%s %s", size, name);
            current.size += size;
        }
    }

    du(root);

    /* return candidates(root); */
    return free_up(root, 30000000 - (70000000 - root.size));
}

int main()
{
    find_space(readText("input.txt").splitLines)
        .writeln;
    return 0;
}

