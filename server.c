#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include "thpool.h"
#include "functions.h"

void main(int argc, char const *argv[])
{
    //Dichiarazione variabili
    struct sockaddr_in address;
    int addrlen = sizeof(address), new_socket;
    char buffer[1024];
    
    initialize();
    load();

    //Gestione segnali
    if (signal(SIGINT, signal_handler) == SIG_ERR)
        printf("Errore nella ricezione di SIGINT\n");
    if (signal(SIGTERM, signal_handler) == SIG_ERR)
        printf("Errore nella ricezione di SIGTERM\n");
    if (signal(SIGQUIT, signal_handler) == SIG_ERR)
        printf("Errore nella ricezione di SIGQUIT\n");
    if (signal(SIGTSTP, signal_handler) == SIG_ERR)
        printf("Errore nella ricezione di SIGTSTP\n");

    thpool = thpool_init(THREADS);

    fprintf(stderr, "Pid: %ld\n", (long)getpid());

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("Errore nella creazione del socket\n");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("Errore nel bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0)
    {
        printf("Errore nel listen\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Errore nell'accept\n");
            exit(EXIT_FAILURE);
        }
        if (thpool_add_work(thpool, (void *)manage, (void *)&new_socket) < 0)
        {
            printf("Errore nell'assegnazione della richiesta alla threadpool\n");
            return;
        }
        //controlla benissimo sta cosa
        if (serverBusy)
        {
            memset(buffer, 0, sizeof(buffer));
            recv(new_socket, buffer, sizeof(buffer), 0);
            printf("Client: %s", buffer);
            strcpy(buffer, "NOK");
            send(new_socket, buffer, sizeof(buffer), 0);
            close(new_socket);
            memset(&new_socket, 0, sizeof(server_fd));
        }
    }
    thpool_destroy(thpool);
    close(server_fd);
}