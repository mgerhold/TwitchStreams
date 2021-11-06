#define NET_USE_CPP
#include "net.h"
#include "security.hpp"
#include <iostream>
#include <cstdlib>

int main() {
	TCPSocket socket;
	socket.connect("localhost", "3321");

	auto stream = socket.stream();

	Message buffer;
	socket.receive_exact(buffer.data(), std::size(buffer));
	const auto key = generateKey();
	const auto encoded = encode(buffer, key);
	socket.send(encoded.data(), std::size(encoded));
	Message answer;
	socket.receive_exact(answer.data(), std::size(answer));
	const auto decoded = encode(answer, key);
	std::cout << toString(decoded) << '\n';
	checkErrors(socket);
}

#define NET_IMPLEMENTATION
#include "net.h"
