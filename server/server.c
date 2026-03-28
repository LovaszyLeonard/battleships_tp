#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "../common/protocol.h"

#define PORT 8080

int main() {
    int server_fd, client_sock;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    Packet p_in, p_out;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    if ((client_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Client connected.\n");

    // 1. Trimite Welcome
    p_out.type = MSG_WELCOME;
    p_out.x = 0;
    p_out.y = 0;
    send(client_sock, &p_out, sizeof(Packet), 0);
    printf("Sent WELCOME.\n");

    // 2. Asteapta focul de test de la client
    if (recv(client_sock, &p_in, sizeof(Packet), 0) > 0) {
        printf("Received packet -> Type: %d, X: %d, Y: %d\n", p_in.type, p_in.x, p_in.y);
        
        // Daca e mesaj de tras, simulam un Miss
        if (p_in.type == MSG_SHOOT) {
            p_out.type = MSG_RESULT_MISS;
            p_out.x = p_in.x;
            p_out.y = p_in.y;
            send(client_sock, &p_out, sizeof(Packet), 0);
            printf("Sent MISS response.\n");
        }
    } else {
        printf("Client disconnected unexpectedly.\n");
    }

    close(client_sock);
    close(server_fd);

    return 0;
}