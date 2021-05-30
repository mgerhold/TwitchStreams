//
// Created by coder2k on 29.05.2021.
//

#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace Hash {

    // https://stackoverflow.com/questions/48896142/is-it-possible-to-get-hash-values-as-compile-time-constants
    /*template <typename Str>
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
    }*/

    [[nodiscard]] std::string_view getStringFromHash(std::size_t hash) noexcept;

    extern std::unordered_map<std::size_t, std::string> cachedHashNames;

    namespace {
        void cacheHashName(std::size_t hash, const std::string &name) noexcept {
            cachedHashNames[hash] = name;
        }
    }

    template <typename Str>
    size_t hashString(const Str& toHash)
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

        cacheHashName(result, std::string{ toHash });

        return result;
    }

}