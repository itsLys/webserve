#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

#include "ClientTable.hpp"
#include "Socket.hpp"

class EventLoop {
   private:
	Socket &_socket;
	ClientTable &_table;
	fd_set _rdset;
	fd_set _wrset;
	int _max_fd;

	void buildFdSets();
	void handleNewConnections(Socket &socket);
	bool handleClientActivity(int clientFd);
	void processClients();

   public:
	EventLoop(Socket &socket, ClientTable &table);
	void setMaxfd(int fd);
	void run();
};

#endif
