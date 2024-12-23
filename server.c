#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFE_SIZE 1024

int handle_client(int client_socket);

int main() {
    int server_socket;
    int client_socket;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    server_socket = socket (AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while(1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        handle_client(client_socket);
    }

    close(server_socket);
    return 0;

}

int handle_client(int client_socket) {
    char *buffer = malloc(BUFFE_SIZE * sizeof(char));
    int bytes_read = read(client_socket, buffer, BUFFE_SIZE - 1);

    if (bytes_read < 0) {
        perror("Read error");
        close(client_socket);
        return 1;
    }

    buffer[bytes_read] = '\0';
    printf("Request:\n%s\n", buffer);

       const char *response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 49\r\n"
        "\r\n"
        "<html><body><h1>Hello, World!</h1></body></html>";
    
    if (send(client_socket, response, strlen(response), 0) < 0) {
        perror("Send error");
    }

    close(client_socket);
    free(buffer);
    return 0;
}
