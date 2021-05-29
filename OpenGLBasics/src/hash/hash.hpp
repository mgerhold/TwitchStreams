//
// Created by coder2k on 29.05.2021.
//

#pragma once

// https://stackoverflow.com/questions/48896142/is-it-possible-to-get-hash-values-as-compile-time-constants
template <typename Str>
constexpr size_t hashString(const Str& toHash)
{
    // For this example, I'm requiring size_t to be 64-bit, but you could
    // easily change the offset and prime used to the appropriate ones
    // based on sizeof(size_t).
    static_assert(sizeof(size_t) == 8);
    // FNV-1a 64 bit algorithm
    size_t result = 0xcbf29ce484222325; // FNV offset basis

    for (char c : toHash) {
        if (c == '\0') {
            break;
        }
        result ^= c;
        result *= 1099511628211; // FNV prime
    }

    return result;
}

/*template<size_t N>
struct ComptimeString {
    constexpr static auto size = N;

    std::array<char, N> chars;

    constexpr ComptimeString(char const (&chars)[N]) {
        std::copy(std::begin(chars), std::end(chars), std::begin(this->chars));
    }

    constexpr auto operator<=>(ComptimeString<N> const&) const = default;

    constexpr operator std::string_view() const {
        return std::string_view{begin(chars), end(chars) - 1};
    }
};

template<size_t N>
ComptimeString(ComptimeString<N>) -> ComptimeString<N>;

template<ComptimeString s>
constexpr auto compileTimeHash = hashString(s);*/

/*template <typename Str>
consteval size_t hashString(const Str& toHash)
{
    return hashStringConstexpr(toHash);
}*/
