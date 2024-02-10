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

#define PORT 8080
#define MAX_CLIENTS 10

int main() {
    int server_socket; // file descriptor for the server socket
    int client_sockets[MAX_CLIENTS]; // file descriptors for the client sockets
    int max_clients = MAX_CLIENTS;
    fd_set readfds; // set of file descriptors to monitor for readability
    int max_sd; // highest-numbered file descriptor in any of the three sets, plus 1
    int activity; // return value of select
    int sd; // file descriptor for the current socket
    int new_socket; // file descriptor for the new socket
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
    for (int i = 0; i < max_clients; i++) {
        client_sockets[i] = 0;
    }

    // set up the server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // bind the server socket to the server address and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Bind error: " << strerror(errno) << std::endl;
        return 1;
    }

    // listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        std::cerr << "Listen error: " << strerror(errno) << std::endl;
        return 1;
    }

    std::cout << "Server started and listening on port " << PORT << std::endl;
    // handle incoming connections and messages from the clients

    while (true)
    {
        FD_ZERO(&readfds); // clear the set of file descriptors means we are not monitoring any file descriptors
        // all the bits in the specified file descriptor set are cleared to 0
        FD_SET(server_socket, &readfds); // add the server socket t/o the set of file descriptors to monitor for readability
        // why : because we want to monitor the server socket for incoming connections
        // and we want to monitor the client sockets for incoming messages
        max_sd = server_socket; // initialize the highest-numbered fd to the server socket

        // add the client sockets to the set of fds to monitor for readability
        for (int i = 0; i < max_clients; i++) {
            sd = client_sockets[i]; // get the current client socket
            if (sd > 0) {
                FD_SET(sd, &readfds); // add the current client socket to the set of file descriptors to monitor for readability
            }
            if (sd > max_sd) {
                max_sd = sd; // update the highest-numbered fd if the current client socket is greater than the current highest-numbered fd
                //because select needs the highest-numbered file descriptor in any of the three sets, plus 1
            }
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL); // call select to monitor the file descriptors for readability
        if (activity < 0) {
            std::cerr << "Select error: " << strerror(errno) << std::endl;
        }
        // else means, which means that at least one file descriptor is ready for reading
        // which means that at least one file descriptor in the readfds set is ready for reading
        // If there is a new incoming connection    
        if (FD_ISSET(server_socket, &readfds)) {
            // FD_ISSET : is a macro that checks if a file descriptor is ready for reading
            // it checs if the bit corresponding to the server_socket is set in the readfds set
            // if the server socket is ready for reading, then it means that there is an incoming connection
            // for each socket we create there is a structure sockaddr_in that contains the address and port of that socket
            int addrlen = sizeof(client_address);
            new_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&addrlen);
            if (new_socket < 0) {
                std::cerr << "Accept error: " << strerror(errno) << std::endl;
            }
            //welcome message
        }
        // Add new socket to array of sockets
        for (int i = 0; i < max_clients; i++) {
            // if position is empty
            if (client_sockets[i] == 0) {
                client_sockets[i] = new_socket;
                FD_SET(new_socket, &readfds);
                int port = ntohs(client_address.sin_port);
                char *ip = inet_ntoa(client_address.sin_addr);
                std::cout << "New connection from socket [" << new_socket << "] with ip (" << ip << ") from port{" << port << "}" << std::endl;
                break;
            }
        }
        // If there is a new incoming message from all the client sockets
        for (int i = 0; i < max_clients; i++) {
            sd = client_sockets[i];
            if (FD_ISSET(sd, &readfds)) {
                memset(buffer, 0, sizeof(buffer));
                int valread = recv(sd, buffer, 1024, 0);
                if (valread == 0) {
                    int port = ntohs(client_address.sin_port);
                    char *ip = inet_ntoa(client_address.sin_addr);
                    std::cout << "Host disconnected, ip (" << ip << ") from port{" << port << "}" << std::endl;
                    close(sd);
                    client_sockets[i] = 0;
                }
                else {
                    buffer[valread - 1] = '\0';
                    std::cout << "{" << buffer << "}" << std::endl;
                    std::string response = "Message recieved\n";
                    write(sd, response.c_str(), response.size());
                }
            }
        }
    }
    
    return 0;
}
