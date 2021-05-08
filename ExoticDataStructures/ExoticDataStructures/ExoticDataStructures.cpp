#include <iostream>
#include <memory>
#include "COLA.h"

using namespace std;

int main() {
	std::cout << "COLA that contains unique_ptr instances\n";
	COLA<std::unique_ptr<int>> colaPtrs;
	auto ptr = std::make_unique<int>(42);	
	colaPtrs.insert(std::move(ptr));
	colaPtrs.insert(std::make_unique<int>(15));
	std::cout << colaPtrs << "\n\n";

	std::cout << "COLA that contains integer values\n";
	COLA<int> cola;
	std::cout << cola << "==============\n";
	cola.insert(20);
	std::cout << cola << "==============\n";
	cola.insert(-420);
	std::cout << cola << "==============\n";
	cola.insert(36);
	std::cout << cola << "==============\n";
	int val = 70;
	cola.insert(val);
	cola.insert(22);
	std::cout << cola << "==============\n";

	std::cout << cola.find(36) << std::endl;	
	std::cout << cola.find(22) << std::endl;
	try {
		std::cout << cola.find(42) << std::endl;
	} catch (...) {
		std::cout << "value not found\n";
	}

	cola.insert(42);
	std::cout << cola << "==============\n";
	std::cout << cola.find(42) << std::endl;
	try {
		std::cout << cola.find(2) << std::endl;
	} catch (...) {
		std::cout << "value not found\n";
	}

	std::cin.get();
}