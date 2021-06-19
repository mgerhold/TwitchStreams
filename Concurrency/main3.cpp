//
// Created by coder2k on 19.06.2021.
//

#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

std::vector<uint64_t> gFibonacciNumbers;
std::mutex gFibonacciVectorMutex;
std::once_flag gFibonacciNumbersGeneratedFlag;

void generateFibonacciNumbers() {
    constexpr std::size_t count = 50;
    gFibonacciNumbers.reserve(count);
    gFibonacciNumbers.push_back(1);
    gFibonacciNumbers.push_back(1);
    for (std::size_t i = 2; i < gFibonacciNumbers.capacity(); ++i) {
        gFibonacciNumbers.push_back(gFibonacciNumbers.at(i - 2) + gFibonacciNumbers.at(i - 1));
    }
}

void doSomething() {
    std::call_once(gFibonacciNumbersGeneratedFlag, generateFibonacciNumbers);
    // alternative version using std::mutex
    /*auto lock = std::unique_lock{ gFibonacciVectorMutex };
    if (gFibonacciNumbers.empty()) {
        generateFibonacciNumbers();
    }
    lock.unlock();*/
    for (int i = 0; i < 5; ++i) {
        std::cout << gFibonacciNumbers.at(20) << '\n';
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(200ms);
    }
}

int main() {
    constexpr std::size_t numberOfThreads = 5;
    std::vector<std::jthread> threads; // jthreads join automatically at the end of the scope
    threads.reserve(numberOfThreads);
    for (std::size_t  i = 0; i < numberOfThreads; ++i) {
        threads.emplace_back(doSomething);
    }
}