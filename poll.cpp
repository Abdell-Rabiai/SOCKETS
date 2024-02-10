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

// using poll
// continuously monitor standard input for readability
// allowing the user to input data within the specified timeout period

int main() {
    while (true) {
        // set up an array of pollfd structures
        struct pollfd fds[1]; // this fds is an array of pollfd structures of size 1 (we will monitor only one file descriptor)
        fds[0].fd = 0; // standard input
        fds[0].events = POLLIN; // monitor only for readability
        // fds[0].events = POLLIN | POLLOUT; // monitor for readability and writability
        // fds[0].events = POLLIN | POLLOUT | POLLERR; // monitor for readability, writability, and error
        // call poll
        int ready = poll(fds, 1, 7000);
        // 1 : is the number of file descriptors in the fds array
        // 7000 : is the timeout in milliseconds in which poll will wait for an event to occur on the file descriptors
        if (ready == -1) {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            break ;
        }
        else if (ready == 0) {
            std::cout << "Timeout occurred! No data after 7 seconds." << std::endl;
        }
        else {
            // here standard input fd is ready for reading
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
}