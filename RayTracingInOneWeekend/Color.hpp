//
// Created by coder2k on 23.10.2021.
//

#pragma once

#include "Vec3.hpp"

#include <iostream>
#include <format>
#include <cmath>

constexpr auto maxColorValue = 255;

void writeColor(std::ostream& outStream, const Color& pixelColor) {
    outStream << std::format("{} {} {}", static_cast<int>(std::round(pixelColor.r * maxColorValue)),
                             static_cast<int>(std::round(pixelColor.g * maxColorValue)),
                             static_cast<int>(std::round(pixelColor.b * maxColorValue)));
}