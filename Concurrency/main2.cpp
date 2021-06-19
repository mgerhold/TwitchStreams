//
// Created by coder2k on 19.06.2021.
//

#include <iostream>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <vector>
#include <thread>
#include <format>
#include <mutex>// mutual exclusion = gegenseitiger Ausschluss
#include <chrono>

double f(const double x) {
    return 1.0 / 20.0 * x * x * x - 11.0 / 20.0 * x * x + 6.0 / 5.0 * x + 3.0;
}

void integrateInterval(const double intervalStart,
                       const double intervalEnd,
                       const uint64_t numberOfStripes,
                       double& refAccumulator,
                       std::mutex& accumulatorMutex) {
    assert(intervalEnd > intervalStart);
    const double stripeWidth = (intervalEnd - intervalStart) / static_cast<double>(numberOfStripes);
    double result = 0.0;
    for (uint64_t i = 0ULL; i < numberOfStripes; ++i) {
        // LERP = linear interpolation
        // lerp(a, b, t) = (1 - t) * a + t * b
        const double currentX = std::lerp(intervalStart + stripeWidth, intervalEnd,
                                          static_cast<double>(i) / static_cast<double>(numberOfStripes));
        result += stripeWidth * f(currentX);
    }
    auto lock = std::scoped_lock{ accumulatorMutex };
    const double oldAccumulatorValue = refAccumulator;
    // sleep for 50 ms to enforce the impact of the race condition
    // std::this_thread::sleep_for(std::chrono::milliseconds(50));
    refAccumulator = oldAccumulatorValue + result;
}

double integrate(const double intervalStart,
                 const double intervalEnd,
                 const uint64_t numberOfStripes,
                 const uint64_t numberOfThreads) {
    assert(numberOfThreads > 0);
    assert(numberOfStripes > 0);
    assert(intervalEnd > intervalStart);
    std::mutex resultMutex;
    double result = 0.0;
    std::vector<std::jthread> workerThreads;
    workerThreads.reserve(numberOfThreads - 1ULL);
    const double subIntervalWidth = (intervalEnd - intervalStart) / static_cast<double>(numberOfThreads);
    const uint64_t numberOfStripesPerThread = numberOfStripes / numberOfThreads;
    for (uint64_t i = 0; i < numberOfThreads - 1ULL; ++i) {
        workerThreads.emplace_back(integrateInterval, intervalStart + static_cast<double>(i) * subIntervalWidth,
                                   intervalStart + static_cast<double>(i + 1) * subIntervalWidth,
                                   numberOfStripesPerThread, std::ref(result), std::ref(resultMutex));
    }
    integrateInterval(intervalStart + static_cast<double>(numberOfThreads - 1ULL) * subIntervalWidth, intervalEnd,
                      numberOfStripesPerThread, result, resultMutex);
    for (auto& thread : workerThreads) {
        thread.join();
    }
    return result;
}

int main() {
    for (uint64_t numThreads = 1ULL; numThreads <= 24ULL; numThreads += 1ULL) {
        const auto startTime = std::chrono::high_resolution_clock::now();
        const double approxArea = integrate(0.0, 10.0, 1'000'000'000ULL, numThreads);
        const auto endTime = std::chrono::high_resolution_clock::now();
        std::cout << std::format("{} {} s ({})\n", numThreads,
                                 std::chrono::duration<double>(endTime - startTime).count(), approxArea);
    }
}