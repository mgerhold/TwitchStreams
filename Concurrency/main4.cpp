//
// Created by coder2k on 19.06.2021.
//

#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <numeric>
#include <shared_mutex>

std::vector<int> gNumbers;
std::shared_mutex gNumbersMutex;

// single writer - multiple readers

void write() {
    using namespace std::chrono_literals;
    for (int i = 0; i < 20; ++i) {
        {
            auto lock = std::scoped_lock{ gNumbersMutex };
            gNumbers.push_back(1);
            std::cout << "=========== Pushed value ===========" << std::endl;
        }
        std::this_thread::sleep_for(25ms);
    }
}

void read() {
    using namespace std::chrono_literals;
    const auto startTime = std::chrono::high_resolution_clock::now();
    while (std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count() < 0.6) {
        {
            auto lock = std::shared_lock{ gNumbersMutex };
            auto sum = std::accumulate(gNumbers.begin(), gNumbers.end(), 0);
            std::cout << std::format("The sum is {}.\n", sum) << std::flush;
        }
        std::this_thread::sleep_for(20ms);
    }
}

int main() {
    std::cout << std::format("Initial vector capacity: {}\n", gNumbers.capacity());
    constexpr std::size_t numReaderThreads = 20;
    std::jthread writerThread{ write };
    std::vector<std::jthread> readerThreads;
    readerThreads.reserve(numReaderThreads);
    for (std::size_t i = 0; i < numReaderThreads; ++i) {
        readerThreads.emplace_back(read);
    }
}