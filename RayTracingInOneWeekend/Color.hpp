//
// Created by coder2k on 23.10.2021.
//

#pragma once

#include "Vec3.hpp"

#include <algorithm>
#include <iostream>
#include <format>
#include <cmath>

constexpr auto maxColorValue = 255;

inline void writeColor(std::ostream& outStream, const Color& pixelColor) {
    constexpr auto maxColorValuePlus1 = maxColorValue + 1;
    outStream << std::format("{} {} {}\n",
                             std::clamp(static_cast<int>(pixelColor.r * (maxColorValuePlus1)), 0, maxColorValue),
                             std::clamp(static_cast<int>(pixelColor.g * (maxColorValuePlus1)), 0, maxColorValue),
                             std::clamp(static_cast<int>(pixelColor.b * (maxColorValuePlus1)), 0, maxColorValue));
}