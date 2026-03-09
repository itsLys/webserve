#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <netinet/in.h>
#include <string>
#include "ClientTable.hpp"

class TcpServer {
   private:
	int _server_fd;
	int _port;
	struct sockaddr_in _addr;

	void createSocket();
	void configureSocket();
	void configureAddress();
	void bindSocket();
	void startListening();
	int acceptClient();
	// void consumeServerSocket(fd_set &rdSet, ClientTable &table);
	// void consumeClientSockets(fd_set &rdSet, ClientTable &table);

   public:
	TcpServer(int port);
	~TcpServer();

	void init();
	void run();
};

#endif
