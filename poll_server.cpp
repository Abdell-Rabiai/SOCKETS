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
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/select.h>

#define PORT 8888
#define MAX_CLIENTS 10
// using poll
int main() {
    int server_socket; // file descriptor for the server socket
    int client_sockets[MAX_CLIENTS]; // file descriptors for the client sockets
    int max_clients = MAX_CLIENTS;
    struct pollfd fds[MAX_CLIENTS + 1]; // this fds is an array of pollfd structures of size MAX_CLIENTS + 1 (we will monitor MAX_CLIENTS client file descriptors and the server file descriptor)
    int activity; // return value of poll
    int sd; // file descriptor for the current socket
    int new_socket; // file descriptor for the new socket
    int i;
    struct sockaddr_in server_address; // structure for the server address
    struct sockaddr_in client_address; // structure for the client address
    char buffer[1024]; // buffer for the incoming data from the client

    // create a server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Socket creation error: " << strerror(errno) << std::endl;
        return 1;
    }

    // initialize the client sockets to 0
    for (i = 0; i < max_clients; i++) {
        client_sockets[i] = 0;
    }

    // set up the server address
    server_address.sin_family = AF_INET; // address family (IPv4)
    server_address.sin_addr.s_addr = INADDR_ANY; // IP address of the machine on which the server is running
    server_address.sin_port = htons(PORT); // port number on which listen for incoming connections

    // bind the server socket to the server address and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Bind error: " << strerror(errno) << std::endl;
        return 1;
    }

    // listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
    // the second parameter is the maximum length to which the queue of pending connections for server_socket may grow
    // when reached the kernel will start rejecting incoming connections
    // this funcion listen will put the server socket in a passive mode, where it waits for the client to approach the server to make a connection
    // essentially the server is saying "I'm ready to accept connections, let me know when you want to connect"
        std::cerr << "Listen error: " << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "Server is listening on port " << PORT << std::endl;

    // set up the server socket in the pollfd structure
    fds[0].fd = server_socket; // server socket
    fds[0].events = POLLIN; // monitor for readability

    // set up the client sockets in the pollfd structure
    for (i = 1; i < max_clients + 1; i++) {
        fds[i].fd = -1; // initialize all client sockets to 0
    }

    while(true) {
        // call poll to monitor the file descriptors for readability
        activity = poll(fds, max_clients + 1, -1); // -1 : wait indefinitely for an event to occur on the file descriptors
        if (activity == -1) {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            break ;
        }
        // if the server socket is readable, then it means that a new connection is incoming
        if (fds[0].revents & POLLIN) {
            int addrlen = sizeof(client_address);
            new_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t*)&addrlen);
            if (new_socket < 0) {
                std::cerr << "Accept error: " << strerror(errno) << std::endl;
                break ;
            }
        }

        //Add new socket to array of sockets
        for (i = 1; i < max_clients + 1; i++) {
            // if the current client socket is 0, then it means that it is not in use
            if (fds[i].fd == -1) {
                fds[i].fd = new_socket; // add the new socket to the pollfd structure
                fds[i].events = POLLIN; // monitor for readability
                std::cout << "New connection, socket fd is " << new_socket << ", ip is : " << inet_ntoa(client_address.sin_addr) << ", port : " << ntohs(client_address.sin_port) << std::endl;
                break ;
            }
        }

        // check for incoming messages from the client sockets 
        for (i = 1; i < max_clients + 1; i++) {
            sd = fds[i].fd;
            if (sd != -1 && fds[i].revents & POLLIN) {
                int bytes_read = read(sd, buffer, sizeof(buffer));
                if (bytes_read == 0) {
                    // somebody disconnected, get his details and print
                    std::cout << "Host disconnected, socket fd is " << sd << ", ip is : " << inet_ntoa(client_address.sin_addr) << ", port : " << ntohs(client_address.sin_port) << std::endl;
                    close(sd);
                    fds[i].fd = -1;
                }
                else {
                    buffer[bytes_read - 1] = '\0';
                    std::cout << "Data recieved on fd " << sd << " : " << buffer << std::endl;
                }
            }
        }
    }
    return 0;
}