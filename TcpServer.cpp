#include "TcpServer.hpp"
#include "ClientTable.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

TcpServer::TcpServer(int port) : _server_fd(-1), _port(port) {}

TcpServer::~TcpServer() {
	if (_server_fd != -1) close(_server_fd);
}

void TcpServer::createSocket() {
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0) {
		std::cerr << "socket() failed\n";
		exit(EXIT_FAILURE);
	}
}

void TcpServer::configureSocket() {
	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
				   sizeof(opt))) {
		std::cout << "setsocket(): " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void TcpServer::configureAddress() {
	std::memset(&_addr, 0, sizeof(_addr));

	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(_port);
	_addr.sin_addr.s_addr = INADDR_ANY;
}

void TcpServer::bindSocket() {
	if (bind(_server_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0) {
		std::cout << "bind(): " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void TcpServer::startListening() {
	if (listen(_server_fd, 10) < 0) {
		std::cout << "listen(): " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

/* return new client fd or -1*/
int TcpServer::acceptClient() {
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);

	int client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &len);

	if (client_fd < 0) {
		std::cout << "accept(): " << std::strerror(errno);
		return -1;
	}

	std::cout << "Client connected: ";
	std::cout << inet_ntoa(client_addr.sin_addr) << ":"
			  << ntohs(client_addr.sin_port);
	std::cout << ' ' << client_fd << std::endl;

	return client_fd;
}

void TcpServer::init() {
	createSocket();
	configureSocket();
	configureAddress();
	bindSocket();
	startListening();

	std::cout << "Listening on port: " << _port << std::endl;
}

void build_fd_sets(int server_fd, fd_set &rdset, fd_set &wrset,
				   ClientMap &map) {
	FD_ZERO(&rdset);
	FD_ZERO(&wrset);
	for (ClientMap::iterator it = map.begin(); it != map.end(); ++it) {
		FD_SET(it->first, &rdset);
		FD_SET(it->first, &wrset);
	}
	FD_SET(server_fd, &rdset);
}

#include <fcntl.h>

void make_non_blocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) return;

	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void TcpServer::run() {
	fd_set rdset;
	fd_set wrset;
	fd_set masterset;
	ClientTable table;
	int max_fd = _server_fd;
	int client_fd;

	FD_ZERO(&masterset);
	FD_ZERO(&rdset);

	while (true) {
		ClientMap &map = table.getAll();
		build_fd_sets(_server_fd, rdset, wrset, map);
		select(max_fd + 1, &rdset, NULL, NULL, NULL);

		if (FD_ISSET(_server_fd, &rdset)) {
			client_fd = acceptClient();

			if (client_fd >= 0) {
				make_non_blocking(client_fd);
				table.add(client_fd);
			}
			if (client_fd > max_fd) max_fd = client_fd;
		}

		for (ClientMap::iterator it = map.begin(); it != map.end();) {
			if (FD_ISSET(it->first, &rdset)) {
				if (it->second->onReadable() == false) {
					std::cout << it->first << ": Disconnected\n";
					it = table.remove(it->first);
					continue;
				}
			}
			if (FD_ISSET(it->first, &wrset)) {
				if (it->second->onWritable() == false) {
					std::cout << it->first << ": Disconnected\n";
					it = table.remove(it->first);
					continue;
				}
			}
			++it;
		}
	}
}
