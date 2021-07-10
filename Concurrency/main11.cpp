//
// Created by coder2k on 10.07.2021.
//

#include <atomic>
#include <iostream>
#include <format>

int main() {
    std::atomic<int> number = 2;
    number.store(10);
    int n = (number = 5); // (number = 5) returns a value, not a reference!
    n = number.exchange(42); // stores 42 in number and returns the previous value of number
    n = (number += 8);
    n = number.fetch_add(10); // fetch_add returns the previous value of number
    std::cout << std::format("n = {}, number = {}\n", n, number.load());
}
