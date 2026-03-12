#include "TcpServer.hpp"
#include <iostream>

TcpServer::TcpServer(int port) : _socket(port), _eventLoop(_socket, _table) {}

void TcpServer::init() {
	_socket.createSocket();
	_socket.configureSocket();
	_socket.configureAddress();
	_socket.bindSocket();
	_socket.startListening();
	std::cout << "Server initialized" << std::endl;
}

void TcpServer::run() {
	_eventLoop.setMaxfd(_socket.getFd());
	while (true) {
		_eventLoop.run();
	}
}
