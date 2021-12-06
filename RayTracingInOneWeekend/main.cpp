#include "Color.hpp"
#include "Ray.hpp"
#include "Hittable.hpp"
#include "Sphere.hpp"
#include "Camera.hpp"
#include "Utility.hpp"
#include "stb_image_write.h"
#include <chrono>
#include <deque>
#include <iostream>
#include <fstream>
#include <functional>
#include <format>
#include <limits>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
#include <cstdint>
#include <cstring>

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

[[nodiscard]] auto createWorkerLambda(int startLine,
                                      int endLine,
                                      int imageWidth,
                                      int imageHeight,
                                      const World& world,
                                      std::vector<std::uint8_t>& imageBuffer,
                                      std::mutex& imageBufferMutex,
                                      int samplesPerPixel,
                                      int maxDepth) {
    return [imageWidth, imageHeight, &world, localBuffer = imageBuffer, &imageBuffer, &imageBufferMutex,
            samplesPerPixel, startLine, endLine, maxDepth]() mutable {
        std::cout << std::format("Calculating from line {} to line {}...\n", startLine, endLine) << std::flush;
        for (auto y = startLine; y <= endLine; ++y) {
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
                writeColor(localBuffer, imageWidth, x, y, pixelColor);
            }
        }
        constexpr auto pixelSize = sizeof(std::uint8_t) * 4;
        const auto offset = (imageWidth * startLine) * pixelSize;
        const auto size = (endLine - startLine + 1) * imageWidth * pixelSize;
        {
            // write the data into the outside buffer
            auto bufferLock = std::scoped_lock{ imageBufferMutex };
            std::memcpy(imageBuffer.data() + offset, localBuffer.data() + offset, size);
        }
        std::cout << std::format("Finished calculating lines {} to {}...\n", startLine, endLine) << std::flush;
    };
}

int main() {
    // image dimensions
    constexpr auto imageWidth = 1200;
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
            if ((center - Point3{ 4.0, 0.2, 0.0 }).length() > 0.9) {
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

    std::vector<std::uint8_t> imageBuffer(static_cast<std::size_t>(imageWidth * imageHeight * 4));
    std::mutex imageBufferMutex;

    const auto numThreads =
            (std::thread::hardware_concurrency() == 0 ? 4 : std::thread::hardware_concurrency() * 7 / 8);
    const auto numTasks = numThreads * 2;
    const int linesPerTask = imageHeight / numTasks;
    std::deque<std::function<void(void)>> tasks;

    int currentEndLine = linesPerTask;
    while (currentEndLine < imageHeight - 1) {
        const auto newEndLine = std::min(currentEndLine + linesPerTask, imageHeight - 1);
        tasks.push_back(createWorkerLambda(currentEndLine, newEndLine, imageWidth, imageHeight, world, imageBuffer,
                                           imageBufferMutex, samplesPerPixel, maxDepth));
        currentEndLine = newEndLine;
    }
    std::mutex mTasksMutex;
    std::cout << std::format("Total image height: {}\n", imageHeight);
    std::vector<std::jthread> workerThreads;
    for (std::remove_cv_t<decltype(numThreads)> i = 0; i < numThreads; ++i) {
        workerThreads.emplace_back([&tasks, &mTasksMutex]() {
            while (true) {
                std::function<void(void)> task;
                {
                    auto lock = std::scoped_lock{ mTasksMutex };
                    if (tasks.empty()) {
                        break;
                    }
                    task = std::move(tasks.front());
                    tasks.pop_front();
                }
                task();
            }
        });
    }
    for (auto& thread : workerThreads) {
        thread.join();
    }
    const auto endTime = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration<double>(endTime - startTime).count();
    std::cerr << std::format("Elapsed time: {} s\n", duration);
    stbi_flip_vertically_on_write(true);
    const auto result = stbi_write_png("raytracer.png", imageWidth, imageHeight, 4, imageBuffer.data(), 4 * imageWidth);
    assert(result);
}
