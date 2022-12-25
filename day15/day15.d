import std.algorithm;
import std.conv;
import std.file;
import std.format;
import std.math;
import std.range;
import std.stdio;
import std.string;
import std.typecons;

struct Sensor
{
    int x;
    int y;
    int r;
}

struct Beacon
{
    int x;
    int y;
}

int distance(int dx, int dy, int sx, int sy)
{
    return (dx - sx).abs + (dy - sy).abs;
}

alias SB = Tuple!(Sensor, Beacon);

SB read_sensor(string input)
{
    Sensor sensor;
    Beacon beacon;
    input.formattedRead("Sensor at x=%s, y=%s: closest beacon is at x=%s, y=%s",
            sensor.x, sensor.y, beacon.x, beacon.y);
    sensor.r = distance(beacon.x, beacon.y, sensor.x, sensor.y);
    return SB(sensor, beacon);
}

bool less_sensor(Sensor lhs, Sensor rhs)
{
    return lhs.y < rhs.y;
}

SB[] load_sensors(string input)
{
    return readText(input).splitLines
            .map!read_sensor
            .array
            /* .sort!less_sensor.release */
            ;
}

int no_beacons(SB[] sbs, int y)
{
    auto potential = sbs.filter!((s) => (y - s[0].y).abs <= s[0].r).array;

    int min_x = 0;
    int max_x = 0;
    int max_r = 0;
    foreach (s; potential)
    {
        min_x = min(min_x, s[0].x);
        max_x = max(max_x, s[0].x);
        max_r = max(max_r, s[0].r);
    }

    int in_range = 0;
    foreach (x; min_x - max_r .. max_x + max_r + 1)
    {
        foreach (s; potential)
        {
            if (x == s[1].x && y == s[1].y)
            {
                continue;
            }
            if (distance(x, y, s[0].x, s[0].y) <= s[0].r)
            {
                ++in_range;
                break;
            }
        }
    }
    return in_range;
}

string distress(SB[] sbs, int maximum)
{
    foreach(y; 0 .. maximum + 1)
    {
        if (y % 10000 == 0)
            y.writeln;
position:
        for(int x = 0; x <= maximum; ++x)
        {
            foreach (s; sbs)
            {
                if (distance(x, y, s[0].x, s[0].y) <= s[0].r)
                {
                    /* [x, y].writeln; */
                    /* s[0].writeln; */
                    x = s[0].x + s[0].r - (y - s[0].y).abs;
                    /* x.writeln; */
                    continue position;
                }
            }
            [x, y].writeln;
            return (x * 4 + (y / 1000000)).to!string ~ (y % 1000000).to!string;
        }
    }
    return "XX";
}



int main()
{
    /* load_sensors("input.txt").no_beacons(2000000).writeln; */
    load_sensors("input.txt").distress(4000000).writeln;
    return 0;
}

unittest
{
    assert(distance(-2, -4, 4, 2) == 12);
    auto sensors = load_sensors("sample.txt");
    assert(sensors.no_beacons(10) == 26);
    assert(sensors.distress(20) == 56000011);
}




