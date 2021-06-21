//
// Created by coder2k on 21.06.2021.
//

#include <iostream>
#include <queue>
#include <string>
#include <cstdlib>
#include <mutex>
#include <string_view>
#include <format>
#include <condition_variable>

std::queue<std::string> gQueue;
std::mutex gQueueMutex;
std::condition_variable gQueueConditionVariable;

std::string generateRandomString() {
    std::string result;
    const int stringLength = rand() % 50 + 5;
    for (int i = 0; i < stringLength; ++i) {
        result += static_cast<char>('A' + rand() % 26);
    }
    return result;
}

[[noreturn]] void produce() {
    std::vector<std::string> randomStrings;
    randomStrings.reserve(3);
    while (true) {
        randomStrings.emplace_back(generateRandomString());
        randomStrings.emplace_back(generateRandomString());
        randomStrings.emplace_back(generateRandomString());
        for (const auto& randomString : randomStrings) {
            std::cout << std::format("Random string: {}\n", randomString);
        }
        std::cout << std::flush;
        {
            auto lock = std::scoped_lock{ gQueueMutex };
            for (auto& randomString : randomStrings) {
                gQueue.push(std::move(randomString));
            }
        }
        gQueueConditionVariable.notify_one();
        randomStrings.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 500 + 1000));
    }
}

void processData(std::string_view value) {
    std::cout << std::format("The length of the string is {}\n", value.size()) << std::flush;
}

static inline auto pop() {
    auto result{ std::move(gQueue.front()) };
    gQueue.pop();
    return result;
}

[[noreturn]] void consume() {
    using namespace std::chrono_literals;
    std::vector<std::string> data;
    data.reserve(8);
    while (true) {
        auto lock = std::unique_lock{ gQueueMutex };
        gQueueConditionVariable.wait(lock, []{ return !gQueue.empty(); });
        do {
            data.emplace_back(pop());
        } while(!gQueue.empty());
        lock.unlock();
        for (const auto& string : data) {
            processData(string);
        }
        data.clear();
    }
}

int main() {
    std::jthread producerThread{ produce };
    std::jthread consumerThread{ consume };
}