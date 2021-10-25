//
// Created by coder2k on 24.10.2021.
//

#pragma once

#include "Ray.hpp"
#include "Utility.hpp"
#include <cmath>

class Camera {
public:
    [[nodiscard]] static Ray getRay(const double s, const double t) {
        const auto randomVecInsideRadiusSizedDisk = lensRadius * Random::randomInsideUnitDisk();
        const auto offset = u * randomVecInsideRadiusSizedDisk.x + v * randomVecInsideRadiusSizedDisk.y;
        const auto rayStartPosition = origin + offset;
        return Ray{ rayStartPosition,
                    lowerLeftCorner + s * horizontalDimension + t * verticalDimension - rayStartPosition };
    }

public:
    static constexpr auto verticalFOV = 20.0;
    static constexpr auto aspectRatio = 3.0 / 2.0; //16.0 / 9.0;

private:
    static constexpr auto theta = toRadians(verticalFOV);
    static inline const auto halfVerticalHeight = std::tan(theta / 2.0);
    static constexpr auto lookFrom = Vec3{ 13.0, 2.0, 3.0 };
    static constexpr auto lookAt = Vec3{ 0.0, 0.0, 0.0 };
    static constexpr auto up = Vec3{ 0.0, 1.0, 0.0 };
    static inline const auto w = (lookFrom - lookAt).normalized();
    static inline const auto u = up.cross(w).normalized();
    static inline const auto v = w.cross(u);

    static constexpr auto aperture = 0.1;
    static constexpr auto lensRadius = aperture / 2.0;
    static inline const auto focusDistance = 10.0;//(lookAt - lookFrom).length();

    static inline const auto viewportHeight = 2.0 * halfVerticalHeight;
    static inline const auto viewportWidth = aspectRatio * viewportHeight;
    static inline const auto origin = lookFrom;
    static inline const auto horizontalDimension = focusDistance * viewportWidth * u;
    static inline const auto verticalDimension = focusDistance * viewportHeight * v;
    static inline const auto lowerLeftCorner =
            origin - horizontalDimension / 2 - verticalDimension / 2 - focusDistance * w;
};