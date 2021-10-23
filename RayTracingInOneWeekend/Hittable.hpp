//
// Created by coder2k on 23.10.2021.
//

#pragma once

#include "Ray.hpp"
#include <optional>

class Hittable {
public:
    virtual ~Hittable() = default;

    [[nodiscard]] virtual std::optional<double> hit(const Ray& ray, double tMin, double tMax) const = 0;
    [[nodiscard]] virtual Vec3 normal(const Point3& intersectionPoint) const = 0;
};