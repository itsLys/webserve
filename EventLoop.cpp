#include "EventLoop.hpp"
#include <fcntl.h>
#include <cstdlib>
#include <iostream>

#include <sys/epoll.h>
#define MAX_EVENTS 128
// NOTE: helper

void make_non_blocking(int fd);
EventLoop::EventLoop(Socket &socket, ClientTable &table)
	: _socket(socket), _table(table) {}

void EventLoop::handleNewConnections(Socket &socket) {
	struct epoll_event ev;
	int clientFd;
	while ((clientFd = socket.acceptClient()) >= 0) {
		make_non_blocking(clientFd);
		_table.add(clientFd);
		ev.events = EPOLLIN;
		ev.data.fd = clientFd;
		if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
			std::cerr << "epoll_ctl: conn_sock\n";
			exit(EXIT_FAILURE);
		}
	}
}

void EventLoop::processClients(struct epoll_event &ev) {
	if (ev.events | EPOLLIN && !_table.get(ev.data.fd)->onReadable()) {
		std::cout << ev.data.fd << ": Disconnected\n";
		epoll_ctl(_epollfd, EPOLL_CTL_DEL, ev.data.fd, NULL);
		_table.remove(ev.data.fd);
		// close connection;
	}
	if (ev.events | EPOLLOUT && !_table.get(ev.data.fd)->onWritable()) {
		std::cout << ev.data.fd << ": Disconnected\n";
		epoll_ctl(_epollfd, EPOLL_CTL_DEL, ev.data.fd, NULL);
		_table.remove(ev.data.fd);
	}
	if (_table.get(ev.data.fd)->hasDataToWrite()) {
		ev.events = ev.events | EPOLLOUT;
		epoll_ctl(_epollfd, EPOLL_CTL_MOD, ev.data.fd, &ev);
	}
}

void EventLoop::loop() {
	struct epoll_event ev;
	struct epoll_event events[MAX_EVENTS];
	int nfds;
	_epollfd = epoll_create1(0);
	if (_epollfd == -1) {
		std::cerr << "epoll_create()\n";  // TODO: print_error_helper;
		std::exit(EXIT_FAILURE);
	}
	ev.events = EPOLLIN;
	ev.data.fd = _socket.getFd();
	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, _socket.getFd(), &ev) == -1) {
		std::cerr << "epoll_ctl: listen_sock\n";
		exit(EXIT_FAILURE);
	}
	while (true) {
		nfds = epoll_wait(_epollfd, events, MAX_EVENTS, -1 /* time out */);
		if (nfds == -1) {
			std::cerr << "epoll_wait\n";
			exit(EXIT_FAILURE);
		}
		for (int n = 0; n < nfds; ++n) {
			if (events[n].data.fd == _socket.getFd()) {
				handleNewConnections(_socket);
			} else {
				processClients(events[n]);
			}
		}
	}
}
