#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <iostream>

// NOTE: helper
void make_non_blocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) {
		std::cerr <<  std::strerror(errno) << std::endl;
		std::cerr << "fcntl() failed\n";
		return;
	} 

	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
