#include <array>
#include <random>
#include <string_view>
#include <cstdint>
#include <cassert>

static constexpr std::size_t length = 16;
using Message = std::array<std::uint8_t, length>;

inline void printMessage(const Message& message) {
	for (const auto element : message) {
		std::cout << static_cast<int>(element) << '\n';
	}
	std::cout << '\n';
}

inline void checkErrors(TCPSocket& socket) {
	if (socket.error() != SocketError_None && socket.error() != SocketError_Closed) {
		std::cout << "Socket error: " << socket.error() << '\n';
	} else {
		std::cout << "No error :)\n";
	}
}

inline Message generateKey() {
	Message result;
	std::mt19937 generator;
	std::uniform_int_distribution<std::uint8_t> distribution;
	for (auto& element : result) {
		element = distribution(generator);
	}
	return result;
}

inline Message toMessage(const std::string_view string) {
	Message result;
	assert(string.length() <= result.size());
	for (std::size_t i = 0; i < result.size(); ++i) {
		result[i] = (i < string.length() ? string[i] : ' ');
	}
	return result;
}

inline std::string toString(const Message& message) {
	std::string result;
	for (const auto element : message) {
		result += element;
	}
	return result.substr(0, result.rfind(' '));
}

inline Message encode(const Message& message, const Message& key) {
	Message result;
	for (std::size_t i = 0; i < message.size(); ++i) {
		result[i] = message[i] ^ key[i];
	}
	// message: 10111011010110
	// key    : 11101101101011
	// xor    : 01010110111101
	return result;
}
