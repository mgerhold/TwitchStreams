//
// Created by coder2k on 10.07.2021.
//

#include <atomic>
#include <iostream>
#include <thread>

static std::atomic<bool> a = false, b = false;

void writeAThenB() {
    a.store(true, std::memory_order_relaxed);
    b.store(true, std::memory_order_relaxed);
}

void readBThenA() {
    while (!b.load(std::memory_order_relaxed));
    if (a.load(std::memory_order_relaxed)) {
        std::cout << "readBThenA(): a and b are true!\n";
    } else {// <- this branch is possible on some platforms
        std::cout << "============= readBThenA(): a is false, b is true!\n";
    }
}

int main() {
    std::jthread reader2{ readBThenA };
    std::jthread writer{ writeAThenB };
}