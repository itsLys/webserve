#include "Socket.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

Socket::Socket(int port) : _server_fd(-1), _port(port) {}

Socket::~Socket() {
	if (_server_fd != -1) close(_server_fd);
}

void Socket::createSocket() {
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0) {
		std::cerr << "socket() failed\n";
		exit(EXIT_FAILURE);
	}
}

void Socket::configureSocket() {
	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
				   sizeof(opt))) {
		std::cout << "setsocket(): " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Socket::configureAddress() {
	std::memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(_port);
	_addr.sin_addr.s_addr = INADDR_ANY;
}

void Socket::bindSocket() {
	if (bind(_server_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0) {
		std::cout << "bind(): " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

#define QUEUE_SIZE 10
void Socket::startListening() {
	if (listen(_server_fd, QUEUE_SIZE) < 0) {
		std::cout << "listen(): " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Listening on port: " << _port << std::endl;
}

int Socket::acceptClient() {
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);

	int client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &len);

	if (client_fd < 0) return -1;

	// TODO: Logging system
	std::cout << "Client connected: ";
	std::cout << inet_ntoa(client_addr.sin_addr) << ":"
			  << ntohs(client_addr.sin_port);
	std::cout << ' ' << client_fd << std::endl;

	return client_fd;
}

int Socket::getFd() { return _server_fd; }
