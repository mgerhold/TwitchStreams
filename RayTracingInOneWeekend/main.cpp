#include "Color.hpp"
#include "Ray.hpp"
#include "Hittable.hpp"
#include "Sphere.hpp"
#include "Camera.hpp"
#include "Utility.hpp"
#include <iostream>
#include <fstream>
#include <format>
#include <limits>
#include <vector>
#include <memory>

using World = std::vector<std::unique_ptr<Hittable>>;

[[nodiscard]] Color backgroundGradient(const Ray& ray) {
    const auto normalizedDirection = ray.direction.normalized();
    const auto colorInterpolationParam = 0.5 * (normalizedDirection.y + 1.0);
    return (1.0 - colorInterpolationParam) * Color{ 1.0, 1.0, 1.0 } + colorInterpolationParam * Color{ 0.5, 0.7, 1.0 };
}

[[nodiscard]] Color rayColor(const Ray& ray, const World& world) {
    auto minT = std::numeric_limits<double>::min();
    bool hitSomething = false;
    std::size_t hittableIndex = 0;
    for (std::size_t i = 0; i < world.size(); ++i) {
        const auto hitResult = world[i]->hit(ray, 0.0, std::numeric_limits<double>::max());
        if (hitResult) {
            if (!hitSomething || hitResult.value() < minT) {
                minT = hitResult.value();
                hitSomething = true;
                hittableIndex = i;
            }
        }
    }
    if (!hitSomething) {
        return backgroundGradient(ray);
    }
    const auto outwardsNormal = world[hittableIndex]->normal(ray.evaluate(minT));
    const auto frontFace = outwardsNormal.dot(ray.direction) < 0.0;
    const auto normal = (frontFace ? outwardsNormal : -outwardsNormal);
    return 0.5 * (normal + Vec3{ 1.0, 1.0, 1.0 });
}

int main() {
    // image dimensions
    constexpr auto imageWidth = 400;
    constexpr auto imageHeight = static_cast<int>(imageWidth / Camera::aspectRatio);
    constexpr auto samplesPerPixel = 100;

    // geometry
    World world;
    world.emplace_back(std::make_unique<Sphere>(Point3{ 0.0, 0.0, -1.0 }, 0.5));
    world.emplace_back(std::make_unique<Sphere>(Point3{ 0.0, -100.5, -1.0 }, 100.0));

    std::ofstream outFileStream{ "image.ppm" };
    // header
    outFileStream << std::format("P3\n{} {}\n{}\n", imageWidth, imageHeight, maxColorValue);

    // pixel data
    for (auto y = imageHeight - 1; y >= 0; --y) {
        std::cerr << std::format("Scanlines remaining: {}\n", y) << std::flush;
        for (int x = 0; x < imageWidth; ++x) {
            Color pixelColor{};
            for (int sample = 0; sample < samplesPerPixel; ++sample) {
                const auto u = (static_cast<double>(x) + Random::randomDouble()) / static_cast<double>(imageWidth);
                const auto v = (static_cast<double>(y) + Random::randomDouble()) / static_cast<double>(imageHeight);
                const auto ray = Camera::getRay(u, v);
                pixelColor += rayColor(ray, world);
            }
            writeColor(outFileStream, pixelColor / static_cast<double>(samplesPerPixel));
        }
    }
    outFileStream.close();
}
