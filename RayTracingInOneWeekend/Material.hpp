//
// Created by coder2k on 24.10.2021.
//

#pragma once

#include "Ray.hpp"
#include "Color.hpp"
#include "Utility.hpp"
#include <memory>
#include <optional>

class Material;

struct IntersectionInfo {
    Point3 intersectionPoint;
    Vec3 normal;
    bool isFrontFace;
    std::shared_ptr<Material> material;

    void setFaceNormal(const Ray& ray, const Vec3& outwardsNormal) {
        isFrontFace = outwardsNormal.dot(ray.direction) < 0.0;
        normal = (isFrontFace ? outwardsNormal : -outwardsNormal);
    }
};

struct ScatterResult {
    Color attenuation;
    Ray ray;
};

class Material {
public:
    [[nodiscard]] virtual std::optional<ScatterResult> scatter(const Ray& intersectionRay,
                                                               const IntersectionInfo& intersectionInfo) = 0;
};

class Lambertian : public Material {
public:
    Lambertian(Color albedo) : albedo{ albedo } { }

    std::optional<ScatterResult> scatter(const Ray&, const IntersectionInfo& intersectionInfo) override {
        const auto newRayDirection = [&]() {
            const auto temp = intersectionInfo.normal + Random::randomUnitVector();
            return temp.isNearZero() ? intersectionInfo.normal : temp;
        }();
        // alternatively use Random::randomVecInsideUnitSphere() or Random::randomVecInsideHemisphere(normal)
        // to generate the scattered ray target
        return ScatterResult{ .attenuation{ albedo },
                              .ray{ Ray{ .origin{ intersectionInfo.intersectionPoint },
                                         .direction{ newRayDirection } } } };
    }

public:
    Color albedo;
};

class Metal : public Material {
public:
    Metal(Color albedo, double fuzz) : albedo{ albedo }, fuzz{ fuzz } { }

    std::optional<ScatterResult> scatter(const Ray& intersectionRay,
                                         const IntersectionInfo& intersectionInfo) override {
        const auto reflected = intersectionRay.direction.normalized().reflect(intersectionInfo.normal) +
                               fuzz * Random::randomVecInsideUnitSphere();
        ScatterResult result;
        result.attenuation = albedo;
        result.ray = Ray{ .origin{ intersectionInfo.intersectionPoint }, .direction{ reflected } };
        if (result.ray.direction.dot(intersectionInfo.normal) > 0) {
            return result;
        }
        return {};
    }

public:
    Color albedo;
    double fuzz;
};