#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include "thpool.h"
#include "functions.h"

void main(int argc, char const *argv[]) 
{ 
    struct sockaddr_in address, serv_addr; 
    int sock = 0, valread, choice;
    char buffer[1024], temp[1024], del[2] = " ";
    char *token; 

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        exit(EXIT_FAILURE); 
    }
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        printf("\nConnection Failed \n"); 
        exit(EXIT_FAILURE); 
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    
    do
    {
        printf("Operazioni disponibili:\nBOOK\nCANCEL\nAVAILABLE\nEXIT\n");
        memset(buffer, 0, sizeof(buffer));
        scanf("%s", buffer);
        choice = convertToIntC(buffer);
        if(!(choice >= 1 && choice <= 4))
            printf("Input errato. Riprova!\n");
    } while(!(choice >= 1 && choice <= 4));
    system("clear");
    switch(choice)
    {
        case 1: //BOOK
            book(sock);
            break;
        case 2: //CANCEL - DA NOTARE IL NONSENSE CHE QUA NON CHIEDE AL SERVER SE E' PRONTO
            cancel(sock);
            break;
        case 3: //AVAILABLE
            available(sock);
            break;
        case 4: //EXIT
            strcpy(buffer, "EXIT");
            send(sock, buffer, sizeof(buffer), 0);
            break;
        default:
            break;
    }
    close(sock);
}

//////////////////////////////////////////////////// FUNCTIONS /////////////////////////////////////////////////////////////