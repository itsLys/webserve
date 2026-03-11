#include "Client.hpp"
#include <unistd.h>
#include <iostream>
#include <string>

Client::Client(int socket_fd) : fd(socket_fd) {}

Client::~Client() {
	if (fd >= 0) close(fd);
	std::cout << fd << " closed\n";
}

bool Client::onReadable() {
	char buff[4096];
	int n = read(fd, buff, sizeof(buff));

	if (n <= 0) return false;
	read_buffer.append(buff, n);

	std::cout << fd << ": ";
	std::cout.write(buff, n);
	return true;
}

bool Client::onWritable() {
	int n = write(fd, "Hello\n", 6);

	if (n <= 0) return false;

	return true;
}


bool Client::hasDataToWrite() const { return !write_buffer.empty(); }
