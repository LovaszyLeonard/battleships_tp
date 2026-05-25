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

    int my_board[BOARD_SIZE][BOARD_SIZE];
    int radar_board[BOARD_SIZE][BOARD_SIZE];
    
    init_board(my_board);
    init_board(radar_board);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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

    printf("Listening on port %d...\n", PORT);

    if ((client_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    
    p_out.type = MSG_WELCOME;
    send(client_sock, &p_out, sizeof(Packet), 0);

    printf("Connected! Setting up fleet...\n");
    sleep(1);
    setup_fleet(my_board);

    p_out.type = MSG_READY;
    send(client_sock, &p_out, sizeof(Packet), 0);
    
    printf("Waiting for opponent to be ready...\n");
    
    do {
        recv(client_sock, &p_in, sizeof(Packet), 0);
    } while (p_in.type != MSG_READY);

    printf("Both players ready! Game starts in 2 seconds...\n");
    sleep(2);

    int game_running = 1;
    int is_my_turn = 1;

    while (game_running) {
        draw_ui(my_board, radar_board);

        if (is_my_turn) {
            int r = -1, c = -1;
            
            while (1) {
                printf("Your turn. Row Col: ");
                scanf("%d %d", &r, &c);
                
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF);
                if (ch == EOF) exit(0); 
                
                if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                    break;
                }
                printf("Invalid! Use 0-9.\n");
            }

            p_out.type = MSG_SHOOT;
            p_out.x = r;
            p_out.y = c;
            send(client_sock, &p_out, sizeof(Packet), 0);

            recv(client_sock, &p_in, sizeof(Packet), 0);
            
            if (p_in.type == MSG_RESULT_HIT || p_in.type == MSG_GAME_OVER) {
                radar_board[r][c] = HIT;
                draw_ui(my_board, radar_board);
                printf("Result: HIT!\n");
            } else if (p_in.type == MSG_RESULT_MISS) {
                radar_board[r][c] = MISS;
                draw_ui(my_board, radar_board);
                printf("Result: MISS!\n");
            }

            if (p_in.type == MSG_GAME_OVER) {
                printf("ENEMY FLEET DESTROYED! YOU WIN!\n");
                game_running = 0;
            }
            
            fflush(stdout);
            sleep(2);
            is_my_turn = 0;
        } else {
            printf("Waiting for opponent...\n");
            
            recv(client_sock, &p_in, sizeof(Packet), 0);
            
            if (p_in.type == MSG_SHOOT) {
                int result = process_shot(my_board, p_in.x, p_in.y);
                p_out.type = result;
                p_out.x = p_in.x;
                p_out.y = p_in.y;

                if (check_victory(my_board)) {
                    p_out.type = MSG_GAME_OVER;
                    game_running = 0;
                }

                send(client_sock, &p_out, sizeof(Packet), 0);

                draw_ui(my_board, radar_board);
                printf("Opponent fired at: %d %d\n", p_in.x, p_in.y);
                
                if (game_running == 0) {
                    printf("YOUR FLEET DESTROYED! YOU LOSE!\n");
                }
                
                fflush(stdout);
                sleep(2);
            }
            is_my_turn = 1;
        }
    }

    close(client_sock);
    close(server_fd);
    printf("Game Over.\n");

    return 0;
}