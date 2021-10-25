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
    explicit Lambertian(Color albedo) : albedo{ albedo } { }

    [[nodiscard]] std::optional<ScatterResult> scatter(const Ray&, const IntersectionInfo& intersectionInfo) override {
        const auto newRayDirection = [&]() {
            const auto temp = intersectionInfo.normal + Random::randomUnitVector();
            return temp.isNearZero() ? intersectionInfo.normal : temp;
        }();
        // alternatively use Random::randomVecInsideUnitSphere() or Random::randomVecInsideHemisphere(normal)
        // to generate the scattered ray target
        return ScatterResult{ .attenuation{ albedo },
                              .ray{ Ray{ intersectionInfo.intersectionPoint, newRayDirection } } };
    }

public:
    const Color albedo;
};

class Metal : public Material {
public:
    Metal(Color albedo, double fuzz) : albedo{ albedo }, fuzz{ fuzz } { }

    [[nodiscard]] std::optional<ScatterResult> scatter(const Ray& intersectionRay,
                                                       const IntersectionInfo& intersectionInfo) override {
        const auto reflected = intersectionRay.direction.normalized().reflect(intersectionInfo.normal) +
                               fuzz * Random::randomVecInsideUnitSphere();
        return ScatterResult{ .attenuation{ albedo }, .ray{ Ray{ intersectionInfo.intersectionPoint, reflected } } };
    }

public:
    const Color albedo;
    const double fuzz;
};

class Dielectric : public Material {
public:
    explicit Dielectric(double refractionIndex) : refractionIndex{ refractionIndex } { }

    [[nodiscard]] std::optional<ScatterResult> scatter(const Ray& intersectionRay,
                                                       const IntersectionInfo& intersectionInfo) override {
        constexpr auto airRefractionIndex = 1.0;
        const auto refractionIndexRatio = intersectionInfo.isFrontFace ? (airRefractionIndex / refractionIndex)
                                                                       : (refractionIndex / airRefractionIndex);
        assert(std::abs(intersectionRay.direction.lengthSquared() - 1.0) <= 0.001);
        const auto cosTheta = std::min(-intersectionRay.direction.dot(intersectionInfo.normal), 1.0);
        const auto sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
        const auto cannotRefract = (refractionIndexRatio * sinTheta > 1.0);
        const auto outgoingRayDirection = [&]() {
            if (cannotRefract || reflectance(cosTheta, refractionIndexRatio) > Random::randomDouble()) {
                return intersectionRay.direction.reflect(intersectionInfo.normal);
            }
            return intersectionRay.direction.refract(intersectionInfo.normal, refractionIndexRatio);
        }();
        return ScatterResult{ .attenuation{ Color{ 1.0, 1.0, 1.0 } },
                              .ray{ Ray{ intersectionInfo.intersectionPoint, outgoingRayDirection } } };
    }

public:
    const double refractionIndex;

private:
    [[nodiscard]] static double reflectance(const double cosTheta, const double refractionIndexRatio) {
        auto r0 = (1.0 - refractionIndexRatio) / (1.0 + refractionIndexRatio);
        r0 *= r0;
        return r0 + (1.0 - r0) * std::pow((1.0 - cosTheta), 5.0);
    }
};