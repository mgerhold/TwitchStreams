#include <iostream>
#include <cstdint>

constexpr int unmutableNumber = 42;

struct Gizmo {
	int a;
	float b;
	double c;
};

// RAII - resource acquisition is initialization
template<typename T>
class SmartPointer {
public:
	SmartPointer(T* pointer) : mData{ pointer } { }

	SmartPointer(SmartPointer&& other) noexcept {
		mData = other.mData;
		other.mData = nullptr;
	}

	SmartPointer(const SmartPointer&) = delete;

	SmartPointer& operator=(const SmartPointer&) = delete;

	SmartPointer& operator=(SmartPointer&& other) noexcept {
		if (this == &other) {
			return *this;
		}
		delete mData;
		mData = other.mData;
		other.mData = nullptr;
		return *this;
	}

	~SmartPointer() {
		delete mData;
	}

	T& operator*() const {
		return *mData;
	}

	T* operator->() const {
		return mData;
	}

private:
	T* mData;
};

template<typename T, typename... Args>
SmartPointer<T> makeSmartPointer(Args&&... args) {
	return SmartPointer<T>{ new T{ std::forward<Args>(args)... } };
}

void printValue(SmartPointer<int> pointer) {
	std::cout << (*pointer) << '\n';
}

int main() {
	auto smartPointer = makeSmartPointer<Gizmo>(42, 1.23f, 4.98);
	(*smartPointer).a++;
	smartPointer->a++;
	std::cout << "The a-value of this gizmo is " << smartPointer->a << '\n';
	auto pointerToInt = makeSmartPointer<int>(100);
	std::cout << "The pointer to int points to a value of " << (*pointerToInt) << '\n';
	pointerToInt = std::move(pointerToInt);
	printValue(std::move(pointerToInt));
	// std::cout << (*pointerToInt) << '\n'; // <- use after move
	auto anotherInt = makeSmartPointer<int>(200);
	pointerToInt = std::move(anotherInt);
	std::cout << (*pointerToInt) << '\n';
}
