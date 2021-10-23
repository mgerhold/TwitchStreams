#include "Color.hpp"
#include <iostream>
#include <fstream>
#include <format>

int main() {
    constexpr auto imageWidth = 256;
    constexpr auto imageHeight = 256;

    std::ofstream outFileStream{ "image.ppm" };
    outFileStream << std::format("P3\n{} {}\n{}\n", imageWidth, imageHeight, maxColorValue);

    for (auto y = imageHeight - 1; y >= 0; --y) {
        std::cerr << std::format("Scanlines remaining: {}\n", y) << std::flush;
        for (int x = 0; x < imageWidth; ++x) {
            const Color pixelColor{ static_cast<double>(x) / static_cast<double>(imageWidth - 1),
                                    static_cast<double>(y) / static_cast<double>(imageHeight - 1), 0.25 };
            writeColor(outFileStream, pixelColor);
            outFileStream << '\n';
        }
    }
    outFileStream.close();
}
