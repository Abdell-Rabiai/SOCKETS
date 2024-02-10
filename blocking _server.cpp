#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8888

int main() {
    int serverSocket, newSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    char buffer[1024];

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    if ((newSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("New connection from %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

    while (1) {
        int valread;
        if ((valread = recv(newSocket, buffer, sizeof(buffer), 0)) <= 0) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        buffer[valread] = '\0';
        printf("Client: %s\n", buffer);
    }

    close(serverSocket);
    return 0;
}
