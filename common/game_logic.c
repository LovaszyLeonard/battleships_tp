#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "game_logic.h"

void init_board(int board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = WATER;
        }
    }
}

int place_ship(int board[BOARD_SIZE][BOARD_SIZE], int row, int col, int size, int horizontal) {
    if (horizontal) {
        if (col + size > BOARD_SIZE) return 0;
        for (int i = 0; i < size; i++) {
            if (board[row][col + i] != WATER) return 0;
        }
        for (int i = 0; i < size; i++) {
            board[row][col + i] = SHIP;
        }
    } else {
        if (row + size > BOARD_SIZE) return 0;
        for (int i = 0; i < size; i++) {
            if (board[row + i][col] != WATER) return 0;
        }
        for (int i = 0; i < size; i++) {
            board[row + i][col] = SHIP;
        }
    }
    return 1;
}

int process_shot(int board[BOARD_SIZE][BOARD_SIZE], int row, int col) {
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
        return MSG_RESULT_MISS;
    }

    if (board[row][col] == SHIP) {
        board[row][col] = HIT;
        return MSG_RESULT_HIT;
    } else if (board[row][col] == WATER) {
        board[row][col] = MISS;
        return MSG_RESULT_MISS;
    }

    return MSG_RESULT_MISS;
}

int check_victory(int board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == SHIP) {
                return 0;
            }
        }
    }
    return 1;
}

void print_cell(int state) {
    if (state == WATER)      printf(COLOR_WATER "~ " COLOR_RESET);
    else if (state == SHIP)  printf(COLOR_SHIP  "S " COLOR_RESET);
    else if (state == HIT)   printf(COLOR_HIT   "X " COLOR_RESET);
    else if (state == MISS)  printf(COLOR_MISS  "O " COLOR_RESET);
}

void draw_ui(int my_board[BOARD_SIZE][BOARD_SIZE], int radar_board[BOARD_SIZE][BOARD_SIZE]) {
    printf(CLEAR_SCREEN);
    
    printf("    === MY FLEET ===               === RADAR ===\n");
    printf("   0 1 2 3 4 5 6 7 8 9          0 1 2 3 4 5 6 7 8 9\n");

    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d  ", i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            print_cell(my_board[i][j]);
        }

        printf("       ");

        printf("%d  ", i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            print_cell(radar_board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void setup_fleet(int my_board[BOARD_SIZE][BOARD_SIZE]) {
    int ships[] = {2, 3};
    int num_ships = 2;

    int dummy_radar[BOARD_SIZE][BOARD_SIZE];
    init_board(dummy_radar);

    for (int i = 0; i < num_ships; i++) {
        int placed = 0;
        while (!placed) {
            draw_ui(my_board, dummy_radar);
            printf("Place ship size %d.\n", ships[i]);
            printf("Row Col Orientation(1=H, 0=V): ");

            int r = -1, c = -1, h = -1;
            
            int args_read = scanf("%d %d %d", &r, &c, &h);

            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            if (ch == EOF) exit(0);

            if (args_read != 3) {
                printf("Invalid format! Please enter exactly 3 numbers.\n");
                fflush(stdout);
                sleep(2);
                continue;
            }

            if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE || (h != 0 && h != 1)) {
                printf("Out of bounds or invalid orientation! Try again.\n");
                fflush(stdout);
                sleep(2);
                continue;
            }

            if (place_ship(my_board, r, c, ships[i], h)) {
                placed = 1;
            } else {
                printf("Ship overlaps another or goes off the board! Try again.\n");
                fflush(stdout);
                sleep(2);
            }
        }
    }
    draw_ui(my_board, dummy_radar);
    printf("Fleet ready!\n");
}