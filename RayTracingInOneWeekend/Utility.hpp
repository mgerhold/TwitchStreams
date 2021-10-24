//
// Created by coder2k on 23.10.2021.
//

#pragma once

#include <limits>
#include <random>
#include <numbers>

constexpr auto infinity = std::numeric_limits<double>::infinity();

constexpr auto toRadians(const double degrees) {
    return degrees * std::numbers::pi / 180.0;
}

class Random {
public:
    [[nodiscard]] static double randomDouble() {
        return mDistribution(mRandomEngine);
    }

    [[nodiscard]] static double randomDouble(double minInclusive, double maxExclusive) {
        return minInclusive + randomDouble() * (maxExclusive - minInclusive);
    }

private:
    static inline std::mt19937_64 mRandomEngine{};
    static inline std::uniform_real_distribution<double> mDistribution{0.0, 1.0};
};