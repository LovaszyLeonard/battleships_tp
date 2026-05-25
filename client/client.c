#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "../common/protocol.h"
#include "../common/game_logic.h"

#define PORT 8080
#define SERVER_IP "127.0.0.1"

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    Packet p_in, p_out;

    int my_board[BOARD_SIZE][BOARD_SIZE];
    int radar_board[BOARD_SIZE][BOARD_SIZE];

    init_board(my_board);
    init_board(radar_board);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid IP");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    if (recv(sock, &p_in, sizeof(Packet), 0) <= 0 || p_in.type != MSG_WELCOME) {
        printf("Connection dropped.\n");
        close(sock);
        return -1;
    }

    printf("Connected! Setting up fleet...\n");
    sleep(1);
    setup_fleet(my_board);

    p_out.type = MSG_READY;
    send(sock, &p_out, sizeof(Packet), 0);

    printf("Waiting for opponent to be ready...\n");

    do {
        recv(sock, &p_in, sizeof(Packet), 0);
    } while (p_in.type != MSG_READY);

    printf("Both players ready! Game starts in 2 seconds...\n");
    sleep(2);

    int game_running = 1;
    int is_my_turn = 0;

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
            send(sock, &p_out, sizeof(Packet), 0);

            recv(sock, &p_in, sizeof(Packet), 0);

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

            recv(sock, &p_in, sizeof(Packet), 0);

            if (p_in.type == MSG_SHOOT) {
                int result = process_shot(my_board, p_in.x, p_in.y);
                p_out.type = result;
                p_out.x = p_in.x;
                p_out.y = p_in.y;

                if (check_victory(my_board)) {
                    p_out.type = MSG_GAME_OVER;
                    game_running = 0;
                }

                send(sock, &p_out, sizeof(Packet), 0);

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

    close(sock);
    printf("Game Over.\n");

    return 0;
}