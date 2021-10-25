#include "Color.hpp"
#include "Ray.hpp"
#include "Hittable.hpp"
#include "Sphere.hpp"
#include "Camera.hpp"
#include "Utility.hpp"
#include <chrono>
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

[[nodiscard]] Color rayColor(const Ray& ray, const World& world, int depth) {
    if (depth <= 0) {
        return Color{};
    }
    auto minT = std::numeric_limits<double>::min();
    bool hitSomething = false;
    std::size_t hittableIndex = 0;
    for (std::size_t i = 0; i < world.size(); ++i) {
        const auto hitResult = world[i]->hit(ray, 0.001, std::numeric_limits<double>::max());
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
    const auto intersectionInfo = world[hittableIndex]->getIntersectionInfo(ray, minT);

    const auto scatterResult = intersectionInfo.material->scatter(ray, intersectionInfo);
    if (!scatterResult) {
        return Color{};
    }
    return scatterResult->attenuation * rayColor(scatterResult->ray, world, depth - 1);
}

[[nodiscard]] Color gammaCorrection(const Color& color) {
    return Color{ std::sqrt(color.r), std::sqrt(color.g), std::sqrt(color.b) };
}

int main() {
    // image dimensions
    constexpr auto imageWidth = 1200.0;
    constexpr auto imageHeight = static_cast<int>(imageWidth / Camera::aspectRatio);
    constexpr auto samplesPerPixel = 500;
    constexpr auto maxDepth = 50;

    // generate the world
    World world;
    const auto materialGround = std::make_shared<Lambertian>(Color{ 0.5, 0.5, 0.5 });
    world.emplace_back(std::make_unique<Sphere>(Point3{ 0.0, -1000.0, -1.0 }, 1000.0, materialGround));

    for (int i = -11; i < 11; ++i) {
        for (int j = -11; j < 11; ++j) {
            const auto center = Point3{ static_cast<double>(i) + 0.9 * Random::randomDouble(), 0.2,
                                        static_cast<double>(j) + 0.9 * Random::randomDouble() };
            if((center - Point3{ 4.0, 0.2, 0.0}).length() > 0.9) {
                const auto chooseMat = Random::randomDouble();
                std::shared_ptr<Material> material;
                if (chooseMat < 0.8) {
                    const auto albedo = Random::randomVec3() * Random::randomVec3();
                    material = std::make_shared<Lambertian>(albedo);
                    world.emplace_back(std::make_unique<Sphere>(center, 0.2, material));
                } else if (chooseMat < 0.95) {
                    const auto albedo = Random::randomVec3(0.5, 1.0);
                    const auto fuzz = Random::randomDouble(0.0, 0.5);
                    material = std::make_shared<Metal>(albedo, fuzz);
                    world.emplace_back(std::make_unique<Sphere>(center, 0.2, material));
                } else {
                    material = std::make_shared<Dielectric>(1.5);
                    world.emplace_back(std::make_unique<Sphere>(center, 0.2, material));
                }
            }
        }
    }
    auto material1 = std::make_shared<Dielectric>(1.5);
    world.emplace_back(std::make_unique<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.emplace_back(std::make_unique<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.emplace_back(std::make_unique<Sphere>(Point3(4, 1, 0), 1.0, material3));

    const auto startTime = std::chrono::high_resolution_clock::now();
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
                pixelColor += rayColor(ray, world, maxDepth);
            }
            pixelColor /= static_cast<double>(samplesPerPixel);
            pixelColor = gammaCorrection(pixelColor);
            writeColor(outFileStream, pixelColor);
        }
    }
    outFileStream.close();
    const auto endTime = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration<double>(endTime - startTime).count();
    std::cerr << std::format("Elapsed time: {} s\n", duration);
}
