#include <regex>
#include <sstream>
#include <charconv>
#include <string>

namespace darllen
{
namespace detail
{

template <typename It, typename Arg>
bool to_value(const std::sub_match<It>& match, Arg& value)
{
    // TODO: Reduce copies
    // TODO: Handle different character types
    std::istringstream input(match.str());
    input >> value;
    // TODO: return false on failed read
    return true;
}

template <typename It>
bool to_value(const std::sub_match<It>& match, int& value)
{
    // from string iterator to const char*
    auto start = &(*match.first);
    auto result = std::from_chars(start, start + match.length(), value);
    return result.ec == std::errc{};
}

template <typename It, typename Ch, typename Traits, typename Alloc>
bool to_value(const std::sub_match<It>& match,
              std::basic_string<Ch, Traits, Alloc>& value)
{
    value = match.str();
    return true;
}

template <typename It, typename Alloc, typename Arg>
bool read_impl(const std::match_results<It, Alloc>& matches, size_t current,
        size_t size, Arg& arg)
{
    if (current < size)
    {
        return to_value(matches[current], arg);
    }
    return false;
}

template <typename It, typename Alloc, typename Arg, typename... Args>
bool read_impl(const std::match_results<It, Alloc>& matches, size_t current,
        size_t size, Arg& arg, Args&... args)
{
    if (current < size)
    {
        to_value(matches[current], arg);
        // TODO: return false on failed read
        return read_impl(matches, current + 1, size, args...);
    }
    return false;
}
}

// TODO: Template the match_results allocator
// TODO: Match flags
template <typename It, typename CharT, typename Traits,
          typename... Args>
bool read(const std::basic_regex<CharT, Traits>& format, It begin, It end,
          Args&... args)
{
    std::match_results<It> matches;
    if (std::regex_match(begin, end, matches, format))
    {
        return detail::read_impl(matches, 1, matches.size(), args...);
    }
    return false;
}

template <typename It, typename CharT, typename Traits,
          typename... Args>
bool search_read(const std::basic_regex<CharT, Traits>& format, It begin, It end,
          Args&... args)
{
    std::match_results<It> matches;
    if (std::regex_search(begin, end, matches, format))
    {
        return detail::read_impl(matches, 1, matches.size(), args...);
    }
    return false;
}

bool from_match(const auto& match, auto& value)
{
    return detail::to_value(match, value);
}

}
