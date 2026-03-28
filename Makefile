# Definim compilatorul si flag-urile de erori
CC = gcc
# -Wall si -Wextra ne arata toate avertismentele (foarte util in C)
# -I./common ii spune compilatorului unde sa caute fisierele .h
CFLAGS = -Wall -Wextra -I./common

# Comanda default cand scrii doar "make" in terminal
all: server_app client_app

# Regula pentru compilarea serverului
server_app: server/server.c
	$(CC) $(CFLAGS) server/server.c -o server/server.out

# Regula pentru compilarea clientului
client_app: client/client.c
	$(CC) $(CFLAGS) client/client.c -o client/client.out

# Comanda "make clean" pentru a sterge executabilele vechi
clean:
	rm -f server/*.out client/*.out