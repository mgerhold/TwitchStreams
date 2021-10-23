//
// Created by coder2k on 23.10.2021.
//

#pragma once

#include <cmath>
#include <iostream>
#include <format>


struct Vec3 {
    Vec3() : x{ 0.0 }, y{ 0.0 }, z{ 0.0 } { }
    Vec3(double v0, double v1, double v2) : x{ v0 }, y{ v1 }, z{ v2 } { }

    Vec3 operator-() const {
        return Vec3{ -x, -y, -z };
    }

    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3& operator*=(const Vec3& other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    Vec3& operator*=(const double scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vec3& operator/=(const double scalar) {
        return (*this *= 1.0 / scalar);
    }

    Vec3 operator+(const Vec3& other) const {
        return Vec3{ x + other.x, y + other.y, z + other.z };
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3{ x - other.x, y - other.y, z - other.z };
    }

    Vec3 operator*(const Vec3& other) const {
        return Vec3{ x * other.x, y * other.y, z * other.z };
    }

    Vec3 operator*(const double scalar) const {
        return Vec3{ x * scalar, y * scalar, z * scalar };
    }

    Vec3 operator/(const double scalar) const {
        return (*this) * (1.0 / scalar);
    }

    double length() const {
        return std::sqrt(lengthSquared());
    }

    double lengthSquared() const {
        return x * x + y * y + z * z;
    }

    double dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3 cross(const Vec3& other) const {
        return Vec3{ y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x };
    }

    Vec3 normalized() const {
        return *this / length();
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

inline Vec3 operator*(const double scalar, const Vec3& vector) {
    return vector * scalar;
}

using Point3 = Vec3;
using Color = Vec3;