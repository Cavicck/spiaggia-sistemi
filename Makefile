all: client server

client: client.c thpool.c funzioni.c
	gcc client.c thpool.c funzioni.c -D THPOOL_DEBUG -pthread -o client

server: server.c thpool.c funzioni.c
	gcc server.c thpool.c funzioni.c -D THPOOL_DEBUG -pthread -o server
