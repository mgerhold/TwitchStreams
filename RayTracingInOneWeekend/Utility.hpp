//
// Created by coder2k on 23.10.2021.
//

#pragma once

#include "Vec3.hpp"
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

    [[nodiscard]] static double randomDouble(const double minInclusive, const double maxExclusive) {
        return minInclusive + randomDouble() * (maxExclusive - minInclusive);
    }

    [[nodiscard]] static Vec3 randomVec3() {
        return Vec3{ randomDouble(), randomDouble(), randomDouble() };
    }

    [[nodiscard]] static Vec3 randomVec3(const double minInclusive, const double maxExclusive) {
        return Vec3{ randomDouble(minInclusive, maxExclusive), randomDouble(minInclusive, maxExclusive),
                     randomDouble(minInclusive, maxExclusive) };
    }

    [[nodiscard]] static Vec3 randomVecInsideUnitSphere() {
        while (true) {
            const auto vec = randomVec3(-1.0, 1.0);
            if ((vec.x != 0.0 || vec.y != 0.0 || vec.z != 0.0) && vec.lengthSquared() < 1.0) {
                return vec;
            }
        }
    }

    [[nodiscard]] static Vec3 randomUnitVector() {
        return randomVecInsideUnitSphere().normalized();
    }

    [[nodiscard]] static Vec3 randomVecInsideHemisphere(const Vec3& normal) {
        const auto result = randomVecInsideUnitSphere();
        return normal.dot(result) > 0.0 ? result : -result;
    }

private:
    static inline std::mt19937_64 mRandomEngine{};
    static inline std::uniform_real_distribution<double> mDistribution{ 0.0, 1.0 };
};