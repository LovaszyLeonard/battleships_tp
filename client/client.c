#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "../common/protocol.h"

#define PORT 8080
#define SERVER_IP "127.0.0.1"

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    Packet p_in, p_out;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }

    printf("Connecting to %s:%d...\n", SERVER_IP, PORT);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    // 1. Asteapta Welcome
    if (recv(sock, &p_in, sizeof(Packet), 0) > 0) {
        if (p_in.type == MSG_WELCOME) {
            printf("Received WELCOME. Connected.\n");
            
            // 2. Trimite un foc de test (X=5, Y=7)
            p_out.type = MSG_SHOOT;
            p_out.x = 5;
            p_out.y = 7;
            send(sock, &p_out, sizeof(Packet), 0);
            printf("Sent SHOOT at X:5, Y:7.\n");

            // 3. Asteapta raspunsul (Hit/Miss)
            if (recv(sock, &p_in, sizeof(Packet), 0) > 0) {
                printf("Received response -> Type: %d, X: %d, Y: %d\n", p_in.type, p_in.x, p_in.y);
            }
        }
    } else {
        printf("Connection dropped.\n");
    }

    close(sock);

    return 0;
}