//
// Created by coder2k on 10.07.2021.
//

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

static std::atomic<bool> gStart{ false };

void doWork() {
    while (!gStart);
    std::cout << "Done!\n";
}

int main() {
    constexpr std::size_t numThreads{ 5 };
    std::vector<std::jthread> threads;
    threads.reserve(numThreads);
    for (std::size_t i = 0; i < numThreads; ++i) {
        threads.emplace_back(doWork);
    }
    gStart = true;
}