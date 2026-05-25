CC = gcc
CFLAGS = -Wall -Wextra -I./common

all: server_app client_app

server_app: server/server.c common/game_logic.c
	$(CC) $(CFLAGS) server/server.c common/game_logic.c -o server/server.out

client_app: client/client.c common/game_logic.c
	$(CC) $(CFLAGS) client/client.c common/game_logic.c -o client/client.out

clean:
	rm -f server/server.out client/client.out