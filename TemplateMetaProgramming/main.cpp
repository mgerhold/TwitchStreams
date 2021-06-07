#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <type_traits>

class IntHolder {
public:
    IntHolder(int value) : mValue(value) { }
    int* begin() { return &mValue; }
    int* end() { return &mValue + 1; }
    const int* begin() const { return &mValue; }
    const int* end() const { return &mValue + 1; }
    const int* cbegin() const { return &mValue; }
    const int* cend() const { return &mValue + 1; }

    using value_type = int;

private:
    int mValue;
};

template<typename CollectionType>
using ContainedType =
        std::remove_cv_t<
            typename std::remove_reference_t<
                    decltype(*std::cbegin(std::declval<CollectionType>()))>
        >;

template<typename CollectionType, typename ReturnType = typename ContainedType<CollectionType>>
auto calculateSum(const CollectionType& container) {
    ReturnType result{};
    for (const ReturnType& element : container) {
        result += element;
    }
    return result;
}

int main() {
    int numbersArray[5] = { 1, 2, 3, 4, 5 };
    std::cout << calculateSum(numbersArray) << "\n";

    static_assert(std::is_same_v<ContainedType<IntHolder>, int>);

    ContainedType<std::vector<int>> number = 5;
    std::vector<int> numsVector{ 1, 2, 3, 4, 5 };
    std::vector<double> floatingPointVector{ 1, 2, 3, 4, 5 };
    std::vector<std::string> words{ "Microsoft", "Google", "Apple", "NoJava" };
    std::list<int> numsList{ 1, 2, 3, 4, 5 };
    std::cout << calculateSum(numsVector) << "\n";
    std::cout << calculateSum(floatingPointVector) << "\n";
    std::cout << calculateSum(words) << "\n";
    std::cout << calculateSum(numsList) << "\n";
}
