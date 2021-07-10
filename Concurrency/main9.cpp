//
// Created by coder2k on 10.07.2021.
//

#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>

class SpinLock {
public:
    void lock() {
        while (mFlag.test_and_set());
    }

    void unlock() {
        mFlag.clear();
    }

private:
    std::atomic_flag mFlag;
};

static SpinLock gLock;

void doWork() {
    auto lock = std::scoped_lock{ gLock };
    std::cout << "This thread has locked the spin lock" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

int main() {
    auto thread1 = std::jthread{ doWork };
    auto thread2 = std::jthread{ doWork };
}
