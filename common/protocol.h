#ifndef PROTOCOL_H
#define PROTOCOL_H

#define BOARD_SIZE 10

typedef enum {
    MSG_WELCOME = 1,
    MSG_READY,
    MSG_SHOOT,
    MSG_RESULT_HIT,
    MSG_RESULT_MISS,
    MSG_GAME_OVER
} MessageType;

typedef struct {
    int type;
    int x;
    int y;
} Packet;

#endif