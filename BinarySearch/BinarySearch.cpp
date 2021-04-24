#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <chrono>

constexpr std::size_t N = 100UL;

bool linearSearch(const std::vector<int>& haystack, int needle) {
    for (std::size_t i = 0; i < haystack.size(); ++i) {
        if (haystack[i] == needle) {
            return true;
        }
    }    
    return false; /* 1 */

    // O(n) <- lineare Zeitkomplexität
}


bool binarySearch(const std::vector<int>& haystack, int needle) {
    std::size_t startInclusive = 0UL;
    std::size_t endExclusive = haystack.size();
    while (endExclusive - startInclusive > 1) {
        std::cout << "Interval: [" << startInclusive << "," << endExclusive << ")\n";        
        std::size_t pivotPosition = (startInclusive + endExclusive) / 2UL;
        std::cout << "\tPivot position: " << pivotPosition << " (" << haystack[pivotPosition] << ")\n";
        if (needle == haystack[pivotPosition]) {
            return true;
        }
        if (needle < haystack[pivotPosition]) {
            endExclusive = pivotPosition;
        } else {
            startInclusive = pivotPosition + 1;
        }
    }
    return (needle == haystack[startInclusive]);

    // O(log n) <- logarithmische Zeitkomplexität
}

int main() {    
    std::vector<int> values{};
    values.resize(N);
    std::iota(values.begin(), values.end(), 2);
    int sum = 0;
    auto startTime = std::chrono::high_resolution_clock::now();    
    for (int i = 0; i < 1; ++i) {
        sum += linearSearch(values, N);
    }
    auto endTime = std::chrono::high_resolution_clock::now();    
    std::cout << "Sum: " << sum << std::endl;
    std::cout << "linear search duration: " << std::chrono::duration<double>(endTime - startTime).count() << std::endl;

    sum = 0;
    startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1; ++i) {
        sum += binarySearch(values, N);
    }    
    std::cout << "Sum: " << sum << std::endl;
    endTime = std::chrono::high_resolution_clock::now();    
    std::cout << "binary search duration: " << std::chrono::duration<double>(endTime - startTime).count() << std::endl;
}