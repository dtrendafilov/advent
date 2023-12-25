solution 'advent'
    configurations { 'debug', 'release' }
    flags {
        'ExtraWarnings',
        'FatalWarnings',
        'Symbols',
        'CppLatest',
    }
project 'solution'
    kind 'ConsoleApp'
    language 'C++'
    uuid(os.uuid('day10'))
    location(solution().location)

    pchheader 'precompiled.hxx'
    pchsource 'precompiled.cxx'
    files {
        '*.cxx',
    }

