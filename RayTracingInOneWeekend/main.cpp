#include "Color.hpp"
#include "Ray.hpp"
#include "Hittable.hpp"
#include "Sphere.hpp"
#include <iostream>
#include <fstream>
#include <format>
#include <limits>
#include <vector>
#include <memory>

[[nodiscard]] Color backgroundGradient(const Ray& ray) {
    const auto normalizedDirection = ray.direction.normalized();
    const auto colorInterpolationParam = 0.5 * (normalizedDirection.y + 1.0);
    return (1.0 - colorInterpolationParam) * Color{ 1.0, 1.0, 1.0 } + colorInterpolationParam * Color{ 0.5, 0.7, 1.0 };
}

[[nodiscard]] Color rayColor(const Vec3& normal) {
    return 0.5 * (normal + Vec3{ 1.0, 1.0, 1.0 });
}

int main() {
    constexpr auto aspectRatio = 16.0 / 9.0;

    // image dimensions
    constexpr auto imageWidth = 400;
    constexpr auto imageHeight = static_cast<int>(imageWidth / aspectRatio);

    // viewport
    constexpr auto viewportHeight = 2.0;
    constexpr auto viewportWidth = aspectRatio * viewportHeight;
    constexpr auto focalLength = 1.0;

    constexpr auto origin = Point3{};
    constexpr auto horizontalDimension = Vec3{ viewportWidth, 0.0, 0.0 };
    constexpr auto verticalDimension = Vec3{ 0.0, viewportHeight, 0.0 };
    constexpr auto lowerLeftCorner =
            origin - horizontalDimension / 2 - verticalDimension / 2 - Vec3{ 0.0, 0.0, focalLength };

    // geometry
    std::vector<std::unique_ptr<Hittable>> hittables;
    hittables.emplace_back(std::make_unique<Sphere>(Point3{ 0.0, 0.0, -1.0 }, 0.5));
    hittables.emplace_back(std::make_unique<Sphere>(Point3{ 0.0, -100.5, -1.0 }, 100.0));

    std::ofstream outFileStream{ "image.ppm" };
    // header
    outFileStream << std::format("P3\n{} {}\n{}\n", imageWidth, imageHeight, maxColorValue);

    // pixel data
    for (auto y = imageHeight - 1; y >= 0; --y) {
        std::cerr << std::format("Scanlines remaining: {}\n", y) << std::flush;
        for (int x = 0; x < imageWidth; ++x) {
            const auto u = static_cast<double>(x) / static_cast<double>(imageWidth - 1);
            const auto v = static_cast<double>(y) / static_cast<double>(imageHeight - 1);
            const auto ray =
                    Ray{ .origin{ origin },
                         .direction{ lowerLeftCorner + u * horizontalDimension + v * verticalDimension - origin } };
            auto minT = std::numeric_limits<double>::min();
            bool hitSomething = false;
            std::size_t hittableIndex = 0;

            for (std::size_t i = 0; i < hittables.size(); ++i) {
                const auto hitResult = hittables[i]->hit(ray, 0.0, std::numeric_limits<double>::max());
                if (hitResult) {
                    if (!hitSomething || hitResult.value() < minT) {
                        minT = hitResult.value();
                        hitSomething = true;
                        hittableIndex = i;
                    }
                }
            }

            const auto pixelColor = [&]() {
                if (hitSomething) {
                    const auto outwardsNormal = hittables[hittableIndex]->normal(ray.evaluate(minT));
                    const auto frontFace = outwardsNormal.dot(ray.direction) < 0.0;
                    const auto normal = (frontFace ? outwardsNormal : -outwardsNormal);
                    return rayColor(normal);
                }
                return backgroundGradient(ray);
            }();
            writeColor(outFileStream, pixelColor);
            outFileStream << '\n';
        }
    }
    outFileStream.close();
}
