//
// Created by coder2k on 23.10.2021.
//

#pragma once

#include "Ray.hpp"
#include "Material.hpp"
#include <optional>

class Hittable {
public:
    virtual ~Hittable() = default;

    [[nodiscard]] virtual std::optional<double> hit(const Ray& ray, double tMin, double tMax) const = 0;
    [[nodiscard]] virtual IntersectionInfo getIntersectionInfo(const Ray& ray, double t) const = 0;
};