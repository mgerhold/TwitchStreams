//
// Created by coder2k on 23.10.2021.
//

#pragma once

#include "Hittable.hpp"

class Sphere : public Hittable {
public:
    Sphere() = default;
    Sphere(const Point3& center, const double radius) : center{ center }, radius{ radius } { }

    [[nodiscard]] std::optional<double> hit(const Ray& ray, double tMin, double tMax) const override {
        const auto sphereCenterToRayOrigin = ray.origin - center;
        const auto squaredRayDirectionLength = ray.direction.lengthSquared();
        const auto minusHalfP = -ray.direction.dot(sphereCenterToRayOrigin) / squaredRayDirectionLength;
        const auto q = (sphereCenterToRayOrigin.lengthSquared() - radius * radius) / squaredRayDirectionLength;
        const auto discriminant = minusHalfP * minusHalfP - q;
        if (discriminant <= 0.0) {
            return {};
        }
        const auto sqrtResult = std::sqrt(discriminant);
        const auto t0 = minusHalfP + sqrtResult;
        const auto t1 = minusHalfP - sqrtResult;
        const auto t0Valid = (t0 >= tMin && t0 <= tMax);
        const auto t1Valid = (t1 >= tMin && t1 <= tMax);
        if (t0Valid && t1Valid) {
            return std::min(t0, t1);
        } else if (t0Valid) {
            return t0;
        } else if (t1Valid) {
            return t1;
        }
        return {};
    }

    [[nodiscard]] Vec3 normal(const Point3& intersectionPoint) const override {
        return (intersectionPoint - center).normalized();
    }

public:
    Point3 center;
    double radius;
};