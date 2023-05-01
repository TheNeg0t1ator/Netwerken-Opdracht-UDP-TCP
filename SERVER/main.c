#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int server_socket, client_socket, result;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    char buffer[BUFFER_SIZE];
    int operation, operand1, operand2, answer;

    // Create a TCP socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8888);

    // Bind the socket to the server address
    result = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (result == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    result = listen(server_socket, 1);
    if (result == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server running and listening for connections...\n");

    // Accept incoming connections
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
    if (client_socket == -1) {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }

    printf("Connected to client %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    // Loop to receive and process messages from the client
    while (1) {
        // Receive the message from the client
        memset(buffer, 0, BUFFER_SIZE);
        result = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (result == -1) {
            perror("Error receiving message");
            exit(EXIT_FAILURE);
        } else if (result == 0) {
            break;
        }

        // Parse the message from the client
        sscanf(buffer, "%d %d %d", &operation, &operand1, &operand2);

        // Perform the requested operation
        switch (operation) {
            case 1:
                answer = operand1 + operand2;
                break;
            case 2:
                answer = operand1 - operand2;
                break;
            case 3:
                answer = operand1 * operand2;
                break;
            case 4:
                if (operand2 == 0) {
                    strcpy(buffer, "Error: division by zero");
                    break;
                } else {
                    answer = operand1 / operand2;
                    break;
                }
            default:
                strcpy(buffer, "Error: invalid operation");
                break;
        }

        // Send the answer back to the client
        memset(buffer, 0, BUFFER_SIZE);
        sprintf(buffer, "%d", answer);
        result = send(client_socket, buffer, strlen(buffer), 0);
        if (result == -1) {
            perror("Error sending answer");
            exit(EXIT_FAILURE);
        }

        // Check if the client wants to stop
        if (strcmp(buffer, "STOP\n") == 0) {
            
            break;
		}
	}

close(client_socket);
    close(server_socket);
    
    return 0;
}