#include <iostream>
#include <thread>
#include <chrono>
#include <format>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <limits>
#include <cstdint>

std::string getCurrentThreadID() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}

void sumPart(const auto begin, const auto end, auto& accumulator) {
    auto tempAccumulator = accumulator;
    for (auto it = begin; it != end; ++it) {
        tempAccumulator += *it;
    }
    accumulator = tempAccumulator;
}

[[nodiscard]] auto sum(const auto begin, const auto end, const unsigned int forceNumThreads = 0U) {
    using ValueType = typename decltype(begin)::value_type;
    const size_t numElements = std::distance(begin, end);

    constexpr unsigned int minNumElementsPerThread = 32U;
    const unsigned int desiredMinNumThreads = std::max(static_cast<std::size_t>(1),
                                                       numElements / minNumElementsPerThread);
    const unsigned int numHardwareThreads = std::max(std::thread::hardware_concurrency(), 1U);
    const unsigned int numThreads = forceNumThreads == 0U ? std::min(desiredMinNumThreads, numHardwareThreads)
            : forceNumThreads;
    //std::cout << std::format("Starting {} worker threads...\n", numThreads);

    const size_t numElementsPerThread = numElements / numThreads;
    std::vector<std::jthread> workerThreads;
    std::vector<ValueType> partialSums(numThreads);
    auto rangeBegin = begin;
    for (unsigned int i = 0U; i < numThreads - 1U; ++i) {
        const auto previousRangeBegin = rangeBegin;
        std::advance(rangeBegin, numElementsPerThread);
        const auto rangeEnd = rangeBegin;
        workerThreads.emplace_back(sumPart<decltype(previousRangeBegin), decltype(rangeEnd), ValueType>,
                previousRangeBegin, rangeEnd, std::ref(partialSums.at(i)));
    }
    sumPart<decltype(rangeBegin), decltype(end), ValueType>(rangeBegin, end, partialSums.back());
    for (auto& thread : workerThreads) {
        thread.join();
    }
    ValueType result{};
    for (const auto& partialSum : partialSums) {
        result += partialSum;
    }
    return result;
}


int main() {
    using ValueType = int;
    constexpr std::size_t numberOfInts = static_cast<std::size_t>(std::numeric_limits<int>::max());
    std::vector<ValueType> intsVector(numberOfInts);
    std::cout << "Vector created..." << std::endl;
    std::fill(intsVector.begin(), intsVector.end(), 1);

    for (unsigned int numThreads = 1U; numThreads <= 24U; ++numThreads) {
        const auto start = std::chrono::high_resolution_clock::now();
        const auto sumOfInts = sum(intsVector.cbegin(), intsVector.cend(), numThreads);
        const auto end = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration<double>(end - start).count();
        std::cout << std::format("{} threads: {} s\n", numThreads, duration);
        assert(sumOfInts == numberOfInts);
    }
    //std::cout << sumOfInts << '\n';
}
