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
#include "funzioni.h"

///////////////////////////////////////////////////////// MAIN /////////////////////////////////////////////////////////////////////
void main(int argc, char const *argv[])
{
    int new_socket, valread;
    struct sockaddr_in address;
    int opt = 1, addrlen = sizeof(address);
    char buffer[1024];
    
    initialize();
    load();
    //Gestione segnali
    if (signal(SIGINT, signal_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");
    if (signal(SIGTERM, signal_handler) == SIG_ERR)
        printf("\ncan't catch SIGTERM\n");
    if (signal(SIGQUIT, signal_handler) == SIG_ERR)
        printf("\ncan't catch SIGQUIT\n");
    if (signal(SIGTSTP, signal_handler) == SIG_ERR)
        printf("\ncan't catch SIGTSTP\n");

    master = thpool_init(1);
    thpool = thpool_init(5);
    fprintf(stderr, "Pid: %ld\n", (long)getpid());
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    thpool_add_work(master, (void *)main_task, NULL);

    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Error on accept");
            exit(EXIT_FAILURE);
        }
        if (thpool_add_work(thpool, (void *)manage, (void *)&new_socket) < 0)
        {
            printf("Can't create thpool\n");
            return;
        }
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
    //thpool_destroy(thpool);
    close(server_fd);
}