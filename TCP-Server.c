#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define IP_ADDRESS "127.0.0.1"
#define BACKLOG 5
#define BUFFER_SIZE 1024

int main(void)
{
    int socket_fd;
    int client_fd;
    struct sockaddr_in server_address = {0};
    struct sockaddr_in client_address = {0};
    socklen_t client_len = sizeof(client_address);
    char buffer[BUFFER_SIZE];
    char data[] = "Hello Client";

    // Creating a socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Configuring the server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, IP_ADDRESS, &server_address.sin_addr) <= 0) {
        perror("inet_pton");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // Binding socket to IP address and port
    if (bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("bind");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    printf("Binding on %s:%d\n", IP_ADDRESS, PORT);

    // Start listening for connections
    if (listen(socket_fd, BACKLOG) < 0) {
        perror("listen");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening on %s:%d\n", IP_ADDRESS, PORT);

    // Accepting client connection
    client_fd = accept(socket_fd, (struct sockaddr *)&client_address, &client_len);
    if (client_fd < 0) {
        perror("accept");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted\n");

    // Receiving data from client
    int byte_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (byte_received < 0) {
        perror("recv");
        close(client_fd);
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    if (byte_received == 0) {
        printf("Client disconnected\n");
        close(client_fd);
        close(socket_fd);
        return 0;
    }
    printf("Received data: %.*s\n", byte_received, buffer);

    // Send response to client
    int byte_sent = send(client_fd, data, sizeof(data) - 1,0);
    if (byte_sent < 0) {
        perror("send");
        close(client_fd);
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    printf("Data sent: %d bytes\n", byte_sent);

    // Closing connection
    close(client_fd);
    close(socket_fd);

    return 0;
}