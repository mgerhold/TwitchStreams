//
// Created by coder2k on 24.10.2021.
//

#pragma once

#include "Ray.hpp"

class Camera {
public:
    [[nodiscard]] static Ray getRay(const double u, const double v) {
        return Ray{ .origin{ origin },
                    .direction{ lowerLeftCorner + u * horizontalDimension + v * verticalDimension - origin } };
    }

public:
    static constexpr auto aspectRatio = 16.0 / 9.0;

private:
    static constexpr auto viewportHeight = 2.0;
    static constexpr auto viewportWidth = aspectRatio * viewportHeight;
    static constexpr auto focalLength = 1.0;
    static constexpr auto origin = Point3{};
    static constexpr auto horizontalDimension = Vec3{ viewportWidth, 0.0, 0.0 };
    static constexpr auto verticalDimension = Vec3{ 0.0, viewportHeight, 0.0 };
    static constexpr auto lowerLeftCorner =
            origin - horizontalDimension / 2 - verticalDimension / 2 - Vec3{ 0.0, 0.0, focalLength };
};