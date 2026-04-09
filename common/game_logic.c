#include "game_logic.h"

// Initializeaza tabla umpland-o cu apa (0)
void init_board(int board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = WATER;
        }
    }
}

// Incearca sa plaseze o nava. Returneaza 1 (Succes) sau 0 (Eroare - iese de pe tabla sau se suprapune)
int place_ship(int board[BOARD_SIZE][BOARD_SIZE], int row, int col, int size, int horizontal) {
    // Verificam limitele si suprapunerile
    if (horizontal) {
        if (col + size > BOARD_SIZE) return 0; // Iese de pe tabla in dreapta
        for (int i = 0; i < size; i++) {
            if (board[row][col + i] != WATER) return 0; // Se suprapune
        }
        // Daca am ajuns aici, e valid. Plasem nava.
        for (int i = 0; i < size; i++) {
            board[row][col + i] = SHIP;
        }
    } else {
        if (row + size > BOARD_SIZE) return 0; // Iese de pe tabla in jos
        for (int i = 0; i < size; i++) {
            if (board[row + i][col] != WATER) return 0; // Se suprapune
        }
        // Valid. Plasem nava.
        for (int i = 0; i < size; i++) {
            board[row + i][col] = SHIP;
        }
    }
    return 1;
}

// Primeste coordonatele unui atac si modifica tabla
int process_shot(int board[BOARD_SIZE][BOARD_SIZE], int row, int col) {
    if (board[row][col] == SHIP) {
        board[row][col] = HIT;
        return MSG_RESULT_HIT;   // Returnam direct tipul de mesaj pentru retea
    } 
    else if (board[row][col] == WATER) {
        board[row][col] = MISS;
        return MSG_RESULT_MISS;
    }
    
    // Daca era deja HIT sau MISS
    return -1; 
}

// Verifica daca jucatorul a pierdut toate navele
int check_victory(int board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == SHIP) {
                return 0; // Inca mai are nave in viata
            }
        }
    }
    return 1; // Nu a gasit nicio nava -> Victorie pentru adversar!
}