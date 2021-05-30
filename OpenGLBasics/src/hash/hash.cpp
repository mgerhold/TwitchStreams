//
// Created by coder2k on 29.05.2021.
//

#include "hash.hpp"

namespace Hash {

    std::unordered_map<std::size_t, std::string> cachedHashNames;

    std::string_view getStringFromHash(std::size_t hash) noexcept {
        const auto it = cachedHashNames.find(hash);
        if (it == cachedHashNames.cend()) {
            return "<name to hash not found>";
        } else {
            return it->second;
        }
    }

}