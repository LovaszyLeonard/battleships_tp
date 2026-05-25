#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "protocol.h"

#define WATER 0
#define SHIP  1
#define HIT   2
#define MISS  3

#define COLOR_RESET  "\033[0m"
#define COLOR_WATER  "\033[34m"
#define COLOR_SHIP   "\033[32m"
#define COLOR_HIT    "\033[31m"
#define COLOR_MISS   "\033[37m"
#define CLEAR_SCREEN "\033[H\033[J"

void init_board(int board[BOARD_SIZE][BOARD_SIZE]);
int place_ship(int board[BOARD_SIZE][BOARD_SIZE], int row, int col, int size, int horizontal);
int process_shot(int board[BOARD_SIZE][BOARD_SIZE], int row, int col);
int check_victory(int board[BOARD_SIZE][BOARD_SIZE]);
void draw_ui(int my_board[BOARD_SIZE][BOARD_SIZE], int radar_board[BOARD_SIZE][BOARD_SIZE]);
void setup_fleet(int my_board[BOARD_SIZE][BOARD_SIZE]);

#endif