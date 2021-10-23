#include "Color.hpp"
#include "Ray.hpp"
#include <iostream>
#include <fstream>
#include <format>

Color rayColor(const Ray& ray) {
    const auto normalizedDirection = ray.direction.normalized();
    const auto t = 0.5 * (normalizedDirection.y + 1.0);
    return (1.0 - t) * Color{ 1.0, 1.0, 1.0 } + t * Color{ 0.5, 0.7, 1.0 };
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
            const auto pixelColor = rayColor(ray);
            writeColor(outFileStream, pixelColor);
            outFileStream << '\n';
        }
    }
    outFileStream.close();
}
