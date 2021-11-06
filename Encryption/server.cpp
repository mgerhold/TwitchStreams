#define NET_USE_CPP
#include "net.h"
#include "security.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

int main() {
	TCPSocket socket;
	socket.listen(3321);

	TCPSocket client = socket.accept();
	auto stream = client.stream();

	const std::string text{ "Hallo VIM!" };
	const auto key = generateKey();
	const auto buffer = encode(toMessage(text), key);
	client.send(buffer.data(), std::size(buffer));

	Message answer;
	client.receive_exact(answer.data(), std::size(answer));
	const auto decoded = encode(answer, key);
	client.send(decoded.data(), std::size(decoded));
	checkErrors(socket);
}

#define NET_IMPLEMENTATION
#include "net.h"
