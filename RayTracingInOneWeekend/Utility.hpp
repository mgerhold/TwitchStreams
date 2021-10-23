//
// Created by coder2k on 23.10.2021.
//

#pragma once

#include <limits>

constexpr auto infinity = std::numeric_limits<double>::infinity();

constexpr auto toRadians(const double degrees) {
    return degrees * pi / 180.0;
}