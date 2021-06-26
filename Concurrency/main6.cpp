//
// Created by coder2k on 26.06.2021.
//

#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <numeric>
#include <format>
#include <sstream>

int getValue() {
    using namespace std::chrono_literals;
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::cout << std::format("Thread id: {}\n", ss.str());
    std::this_thread::sleep_for(200ms);
    return 42;
}

int main() {
    constexpr std::size_t numThreads = 5;
    std::vector<std::future<int>> futures;
    futures.reserve(numThreads);
    for (std::size_t i = 0; i < numThreads; ++i) {
        futures.emplace_back(std::async(getValue));
    }
    const int sum = std::accumulate(futures.begin(), futures.end(), 0, [](int a, std::future<int>& b) {
        return a + b.get();
    });
    std::cout << sum << '\n';
}