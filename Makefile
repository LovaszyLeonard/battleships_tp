CC = gcc
CFLAGS = -Wall -Wextra -I./common

all: server_app client_app


# compilarea serverului
server_app: server/server.c common/game_logic.c
	$(CC) $(CFLAGS) server/server.c common/game_logic.c -o server/server.out

# compilarea clientului
client_app: client/client.c common/game_logic.c
	$(CC) $(CFLAGS) client/client.c common/game_logic.c -o client/client.out


# Comanda "make clean" pentru a sterge executabilele vechi
clean:
	rm -f server/*.out client/*.out