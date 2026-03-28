#ifndef PROTOCOL_H
#define PROTOCOL_H

// board size
#define BOARD_SIZE 10

typedef enum {
    MSG_CONNECT = 0,    // Client -> Server: "Connecting..."
    MSG_WELCOME,        // Server -> Client: "Connected. You are P2."
    MSG_READY,          // Both: "Board set."
    MSG_SHOOT,          // Both: "Shooting at X, Y"
    MSG_RESULT_HIT,     // Both: "Hit!"
    MSG_RESULT_MISS,    // Both: "Miss."
    MSG_RESULT_SUNK,    // Both: "Ship sunk!"
    MSG_GAME_OVER       // Both: "Game over."
} MessageType;

typedef struct {
    MessageType type;
    int x;
    int y;
} Packet;

#endif 