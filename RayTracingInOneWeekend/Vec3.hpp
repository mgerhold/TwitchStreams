//
// Created by coder2k on 23.10.2021.
//

#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
#include <format>


struct Vec3 {
    constexpr Vec3() : x{ 0.0 }, y{ 0.0 }, z{ 0.0 } { }
    constexpr Vec3(double v0, double v1, double v2) : x{ v0 }, y{ v1 }, z{ v2 } { }

    [[nodiscard]] constexpr bool operator==(const Vec3& other) const {
        return (x == other.x && y == other.y && z == other.z);
    }

    [[nodiscard]] constexpr Vec3 operator-() const {
        return Vec3{ -x, -y, -z };
    }

    constexpr Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    constexpr Vec3& operator*=(const Vec3& other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    constexpr Vec3& operator*=(const double scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr Vec3& operator/=(const double scalar) {
        return (*this *= 1.0 / scalar);
    }

    [[nodiscard]] constexpr Vec3 operator+(const Vec3& other) const {
        return Vec3{ x + other.x, y + other.y, z + other.z };
    }

    [[nodiscard]] constexpr Vec3 operator-(const Vec3& other) const {
        return Vec3{ x - other.x, y - other.y, z - other.z };
    }

    [[nodiscard]] constexpr Vec3 operator*(const Vec3& other) const {
        return Vec3{ x * other.x, y * other.y, z * other.z };
    }

    [[nodiscard]] constexpr Vec3 operator*(const double scalar) const {
        return Vec3{ x * scalar, y * scalar, z * scalar };
    }

    [[nodiscard]] inline friend constexpr Vec3 operator*(const double scalar, const Vec3& vector) {
        return vector * scalar;
    }

    [[nodiscard]] constexpr Vec3 operator/(const double scalar) const {
        return (*this) * (1.0 / scalar);
    }

    [[nodiscard]] double length() const {
        return std::sqrt(lengthSquared());
    }

    [[nodiscard]] constexpr double lengthSquared() const {
        return x * x + y * y + z * z;
    }

    [[nodiscard]] constexpr double dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    [[nodiscard]] constexpr Vec3 cross(const Vec3& other) const {
        return Vec3{ y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x };
    }

    [[nodiscard]] Vec3 normalized() const {
        return *this / length();
    }

    [[nodiscard]] bool isNearZero() const {
        constexpr auto epsilon = 1e-8;
        return (std::abs(x) < epsilon && std::abs(y) < epsilon && std::abs(z) < epsilon);
    }

    [[nodiscard]] Vec3 reflect(const Vec3& normal) const {
        return *this - 2.0 * this->dot(normal) * normal;
    }

    [[nodiscard]] Vec3 refract(const Vec3& normal, const double refractionIndexRatio) const {
        assert(std::abs(lengthSquared() - 1.0) <= 0.01);
        assert(std::abs(normal.lengthSquared() - 1.0) <= 0.01);
        const auto cosTheta = std::min(-(*this).dot(normal), 1.0);
        const auto outDirectionPerpendicular = refractionIndexRatio * (*this + cosTheta * normal);
        const auto outDirectionParallel =
                -std::sqrt(std::abs(1.0 - outDirectionPerpendicular.lengthSquared())) * normal;
        return outDirectionPerpendicular + outDirectionParallel;
    }

    union {
        double x;
        double r;
    };
    union {
        double y;
        double g;
    };
    union {
        double z;
        double b;
    };
};

inline std::ostream& operator<<(std::ostream& outStream, const Vec3& vector) {
    outStream << std::format("{} {} {}", vector.x, vector.y, vector.z);
    return outStream;
}

using Point3 = Vec3;
using Color = Vec3;
