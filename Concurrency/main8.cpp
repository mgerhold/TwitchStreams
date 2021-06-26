//
// Created by coder2k on 26.06.2021.
//

#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <stdexcept>

void calculate(std::promise<int> promise) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
    promise.set_value(42);
    //promise.set_exception(std::make_exception_ptr(std::logic_error{ "This is not allowed!" }));
}

int main() {
    using namespace std::chrono_literals;
    std::promise<int> promise;
    auto future = promise.get_future();

    // you can share the future to pass copies of it to other threads
    // std::shared_future<int> sharedFuture{ std::move(future) };
    // auto sharedFuture = future.share();

    std::cout << "Starting thread...\n";
    std::jthread thread{ calculate, std::move(promise) };
    const auto timeout = std::chrono::steady_clock::now() + 1500ms;
    auto futureStatus = future.wait_until(timeout);
    if (futureStatus == std::future_status::timeout) {
        std::cout << "Future timed out\n";
    } else if (futureStatus == std::future_status::ready) {
        try {
            std::cout << "Result is: " << future.get() << '\n';
        } catch (std::exception& e) {
            std::cout << "Error: " << e.what() << '\n';
        }
    }
}