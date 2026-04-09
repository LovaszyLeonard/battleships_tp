#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "protocol.h"

// Definim starile posibile ale unei casute de pe tabla
#define WATER 0
#define SHIP  1
#define HIT   2
#define MISS  3

// Functii pentru manipularea tablei
void init_board(int board[BOARD_SIZE][BOARD_SIZE]);
int place_ship(int board[BOARD_SIZE][BOARD_SIZE], int row, int col, int size, int horizontal);
int process_shot(int board[BOARD_SIZE][BOARD_SIZE], int row, int col);
int check_victory(int board[BOARD_SIZE][BOARD_SIZE]);

#endif // GAME_LOGIC_H