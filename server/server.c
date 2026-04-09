#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "../common/protocol.h"
#include "../common/game_logic.h"

#define PORT 8080

int main() {
    int server_fd, client_sock;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    Packet p_in, p_out;

    // --- INITIALIZARE JOC ---
    int my_board[BOARD_SIZE][BOARD_SIZE];
    init_board(my_board);
    
    // nava de test (dimensiune 3, orizontala, la randul 0, coloana 0)
    place_ship(my_board, 0, 0, 3, 1); 

    // --- RETELISTICA (Setup) ---
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
    
    printf("Client connected. Game starting!\n\n");

    // Trimitem confirmarea
    p_out.type = MSG_WELCOME;
    send(client_sock, &p_out, sizeof(Packet), 0);

    // --- GAME LOOP ---
    int game_running = 1;
    int is_my_turn = 1; // Serverul (Player 1) muta primul

    while (game_running) {
        if (is_my_turn) {
            // RUNDA SERVERULUI
            int r, c;
            printf("Your turn. Enter Row and Col (0-9): ");
            scanf("%d %d", &r, &c);

            // Trimitem lovitura
            p_out.type = MSG_SHOOT;
            p_out.x = r;
            p_out.y = c;
            send(client_sock, &p_out, sizeof(Packet), 0);

            // Asteptam rezultatul
            recv(client_sock, &p_in, sizeof(Packet), 0);
            
            if (p_in.type == MSG_RESULT_HIT) {
                printf("Result: HIT!\n\n");
            } else if (p_in.type == MSG_RESULT_MISS) {
                printf("Result: MISS!\n\n");
            } else if (p_in.type == MSG_GAME_OVER) {
                printf("Result: HIT! ALL SHIPS DESTROYED! YOU WIN!\n");
                game_running = 0;
            }
            
            is_my_turn = 0;
        } else {
            // RUNDA CLIENTULUI
            printf("Waiting for opponent...\n");
            
            recv(client_sock, &p_in, sizeof(Packet), 0);
            
            if (p_in.type == MSG_SHOOT) {
                printf("Opponent shot at Row: %d, Col: %d\n", p_in.x, p_in.y);
                
                // Procesam pe tabla noastra
                int result = process_shot(my_board, p_in.x, p_in.y);
                p_out.type = result;
                p_out.x = p_in.x;
                p_out.y = p_in.y;

                // Verificam daca am pierdut
                if (check_victory(my_board)) {
                    p_out.type = MSG_GAME_OVER;
                    game_running = 0;
                }

                // Trimitem rezultatul inapoi
                send(client_sock, &p_out, sizeof(Packet), 0);

                if (game_running == 0) {
                    printf("ALL YOUR SHIPS DESTROYED. YOU LOSE!\n");
                } else {
                    printf("Board updated.\n\n");
                }
            }
            
            is_my_turn = 1; // Ne vine randul
        }
    }

    // Curatenie
    close(client_sock);
    close(server_fd);
    printf("Game over. Server shutting down.\n");

    return 0;
}