# include <iostream>
# include <string>
# include <vector>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <poll.h>
# include <cstring>
# include <arpa/inet.h>
# include <fcntl.h>
# include <errno.h>
# include <sys/types.h>
# include <sys/stat.h>

// using select
//continuously monitor standard input for readability
// allowing the user to input data within the specified timeout period

int main() {

	while(1) {
		// initialize file descriptor set
		fd_set readfds; // this readfds is a set of file descriptors that we want to monitor for readability
		FD_ZERO(&readfds); // clear the set of file descriptors
		FD_SET(0, &readfds); // add standard input to the set of file descriptors to monitor for readability

		// initialize timeout struct
		struct timeval timeout;
		timeout.tv_sec = 7; // seconds
		timeout.tv_usec = 0; // microseconds

		// monitor standard input for readability
		//int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
		int ready = select(1, &readfds, NULL, NULL, &timeout); // the first parameter is the highest-numbered file descriptor in any of the three sets, plus 1
		if (ready == -1) {
			std::cerr << "Select error: " << strerror(errno) << std::endl;
			break ;
		}
		else if (ready == 0) {
			std::cout << "Timeout occurred! No data after 7 seconds." << std::endl;
		}
		else {
			// here standard input is ready for reading
			char buffer[1024];
			int bytes_read = read(0, buffer, sizeof(buffer));
			if (bytes_read > 0) {
				buffer[bytes_read - 1] = '\0'; // ecrase the '\n' character
				std::cout << "Data recieved on fd 0 : {" << buffer << "}" << std::endl;
			}
			else if (bytes_read == 0) {
				std::cout << "EOF" << std::endl;
				break ;
			}
			else {
				std::cerr << "Read error: " << strerror(errno) << std::endl;
				break ;
			}
		}
	}
	return 0;
}
