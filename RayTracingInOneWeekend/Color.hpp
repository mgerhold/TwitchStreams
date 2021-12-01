//
// Created by coder2k on 23.10.2021.
//

#pragma once

#include "Vec3.hpp"

#include <algorithm>
#include <iostream>
#include <format>
#include <vector>
#include <cstdint>
#include <cmath>

constexpr auto maxColorValue = 255;

inline void writeColor(std::vector<std::uint8_t>& imageBuffer,
                       const std::size_t imageWidth,
                       const int x,
                       const int y,
                       const Color& pixelColor) {
    constexpr auto maxColorValuePlus1 = maxColorValue + 1;
    imageBuffer.at(4 * (y * imageWidth + x) + 0) = static_cast<std::uint8_t>(
            std::clamp(static_cast<int>(pixelColor.r * (maxColorValuePlus1)), 0, maxColorValue));
    imageBuffer.at(4 * (y * imageWidth + x) + 1) = static_cast<std::uint8_t>(
            std::clamp(static_cast<int>(pixelColor.g * (maxColorValuePlus1)), 0, maxColorValue));
    imageBuffer.at(4 * (y * imageWidth + x) + 2) = static_cast<std::uint8_t>(
            std::clamp(static_cast<int>(pixelColor.b * (maxColorValuePlus1)), 0, maxColorValue));
    imageBuffer.at(4 * (y * imageWidth + x) + 3) = std::uint8_t{ 255 };
}