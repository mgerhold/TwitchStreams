//
// Created by coder2k on 26.06.2021.
//

#include <iostream>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <format>

std::queue<std::packaged_task<int(void)>> gTaskQueue;
std::mutex gTaskQueueMutex;

[[noreturn]] void handleTasks() {
    using namespace std::chrono_literals;
    for (;;) {
        decltype(gTaskQueue)::value_type task;
        {
            auto lock = std::scoped_lock{ gTaskQueueMutex };
            if (!gTaskQueue.empty()) {
                task = std::move(gTaskQueue.front());
                gTaskQueue.pop();
            }
        }
        if (task.valid()) {
            std::cout << "Invoking task...\n";
            std::this_thread::sleep_for(1s);
            task();
        }
    }
}

int main() {
    using namespace std::chrono_literals;
    std::jthread taskHandler{ handleTasks };
    std::packaged_task task{ [] {
        std::this_thread::sleep_for(500ms);
        return 42;
    } };
    auto future = task.get_future();
    std::cout << "Putting task onto queue...\n";
    {
        auto lock = std::scoped_lock{ gTaskQueueMutex };
        gTaskQueue.push(std::move(task));
    }
    std::cout << "Waiting for result of task...\n";
    std::cout << std::format("The result is: {}", future.get());
}