#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Includem fisierele comune
#include "../common/protocol.h"
#include "../common/game_logic.h"

#define PORT 8080
#define SERVER_IP "127.0.0.1" // Nu uita sa schimbi cu IP-ul serverului daca joci intre VM-uri

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    Packet p_in, p_out;

    // --- INITIALIZARE JOC ---
    int my_board[BOARD_SIZE][BOARD_SIZE];
    init_board(my_board);
    
    // Plasem o nava de test pentru client (dimensiune 3, orizontala, la randul 5, coloana 5)
    place_ship(my_board, 5, 5, 3, 1);

    // --- RETELISTICA (Setup) ---
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

    // Asteptam Welcome
    if (recv(sock, &p_in, sizeof(Packet), 0) > 0 && p_in.type == MSG_WELCOME) {
        printf("Connected to server. Game starting!\n\n");
    } else {
        printf("Failed to start game.\n");
        close(sock);
        return -1;
    }

    // --- GAME LOOP ---
    int game_running = 1;
    int is_my_turn = 0; // Serverul (P1) incepe, deci noi asteptam

    while (game_running) {
        if (is_my_turn) {
            // RUNDA CLIENTULUI
            int r, c;
            printf("Your turn. Enter Row and Col (0-9): ");
            scanf("%d %d", &r, &c);

            // Trimitem atacul
            p_out.type = MSG_SHOOT;
            p_out.x = r;
            p_out.y = c;
            send(sock, &p_out, sizeof(Packet), 0);

            // Asteptam rezultatul (Hit/Miss/Sunk)
            recv(sock, &p_in, sizeof(Packet), 0);
            
            if (p_in.type == MSG_RESULT_HIT) {
                printf("Result: HIT!\n\n");
            } else if (p_in.type == MSG_RESULT_MISS) {
                printf("Result: MISS!\n\n");
            } else if (p_in.type == MSG_GAME_OVER) {
                printf("Result: HIT! ALL SHIPS DESTROYED! YOU WIN!\n");
                game_running = 0; // Iesim din bucla, am castigat
            }

            is_my_turn = 0; // Predam stafeta
        } else {
            // RUNDA SERVERULUI (Noi ne aparam)
            printf("Waiting for opponent...\n");
            
            recv(sock, &p_in, sizeof(Packet), 0);
            
            if (p_in.type == MSG_SHOOT) {
                printf("Opponent shot at Row: %d, Col: %d\n", p_in.x, p_in.y);
                
                // Calculam rezultatul pe tabla noastra
                int result = process_shot(my_board, p_in.x, p_in.y);
                p_out.type = result;
                p_out.x = p_in.x;
                p_out.y = p_in.y;

                // Verificam daca am ramas fara nave
                if (check_victory(my_board)) {
                    p_out.type = MSG_GAME_OVER;
                    game_running = 0;
                }

                // Trimitem rezultatul inapoi serverului
                send(sock, &p_out, sizeof(Packet), 0);

                if (game_running == 0) {
                    printf("ALL YOUR SHIPS HAVE BEEN DESTROYED. YOU LOSE!\n");
                } else {
                    printf("Board updated.\n\n");
                }
            }

            is_my_turn = 1; // E randul nostru sa atacam
        }
    }

    // Curatenie
    close(sock);
    printf("Game over. Client shutting down.\n");

    return 0;
}