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

//funzioni per il server
void *manage(int *sock)
{
    int new_socket = *sock, count, temp, temp1, dateS, dateE;
    char buffer[1024], del[2] = " ";
    char *token, *ptr;
    //fprintf(stderr, "socket: %d\n", new_socket);
    if ((recv(new_socket, buffer, sizeof(buffer), 0)) < 0)
    {
        close(new_socket);
        printf("Connection closed\n");
        exit(-1);
    }
    //fprintf(stderr, "Client: %s\n", buffer);
    if (strcmp(buffer, "BOOK") == 0) //BOOK
    {
        temp = areFull();
        if (temp == 0)
        {
            strcpy(buffer, "NAVAILABLE");
            if ((send(new_socket, buffer, sizeof(buffer), 0)) < 0)
            {
                close(new_socket);
                printf("Connection closed\n");
                exit(-1);
            }
        }
        else
        {
            //fprintf(stderr, "Buffer: %s\n", buffer);
            strcpy(buffer, "OK");
            //fprintf(stderr, "Server: %s\n", buffer);
            if ((send(new_socket, buffer, sizeof(buffer), 0)) < 0)
            {
                close(new_socket);
                printf("Connection closed\n");
                exit(-1);
            }
            if ((recv(new_socket, buffer, sizeof(buffer), 0)) < 0)
            {
                close(new_socket);
                printf("Connection closed\n");
                exit(-1);
            }
            printf("\n################################################################\n");
            fprintf(stderr, "Client: %s\n", buffer);
            token = strtok(buffer, del); //BOOK
            token = strtok(NULL, del);   //Num ombrellone
            temp1 = atoi(token);
            //fprintf(stderr, "Num ombrellone: %d\n", temp1);
            if (isFull(temp1 - 1) == 0)
            {
                strcpy(buffer, "AVAILABLE");
                if ((send(new_socket, buffer, sizeof(buffer), 0)) < 0)
                {
                    close(new_socket);
                    printf("Connection closed\n");
                    exit(-1);
                }
                if ((recv(new_socket, buffer, sizeof(buffer), 0)) < 0)
                {
                    close(new_socket);
                    printf("Connection closed\n");
                    exit(-1);
                }
                temp1 = convertToInt(buffer);
                switch (temp1)
                {
                case 1: //BOOK DA OGGI AD UNA DATA
                    if ((recv(new_socket, buffer, sizeof(buffer), 0)) < 0)
                    {
                        close(new_socket);
                        printf("Connection closed\n");
                        exit(-1);
                    }
                    if (strcmp(buffer, "CORRECT") == 0)
                    {
                        if ((recv(new_socket, buffer, sizeof(buffer), 0)) < 0)
                        {
                            close(new_socket);
                            printf("Connection closed\n");
                            exit(-1);
                        }
                        token = strtok(buffer, del);     //BOOK
                        temp1 = atoi(strtok(NULL, del)); //Num ombrellone

                        dateS = atoi(strtok(NULL, del)); //Data start
                        dateE = atoi(strtok(NULL, del)); //Data end
                        pthread_mutex_lock(&block_ombrellone[temp1 - 1]);
                        temp = addDate(&(ombrelloni[temp1 - 1].prenotazioni), &ombrelloni[temp1 - 1].numPrenotazioni, dateS, dateE);
                        pthread_mutex_unlock(&block_ombrellone[temp1 - 1]);
                        if (temp != 0)
                        {
                            pthread_mutex_lock(&block_log);
                            saveLog(BOOK, temp1, dateS, dateE);
                            pthread_mutex_unlock(&block_log);
                            printf("Prenotazione effettuata\n");
                            memset(buffer, 0, sizeof(buffer));
                            strcpy(buffer, "Prenotazione effettuata");
                            printf("Num prenotazioni %d\n", ombrelloni[temp1 - 1].numPrenotazioni);
                            printOmbrellone(ombrelloni[temp1 - 1].prenotazioni, temp1);
                        }
                        else
                        {
                            printf("Data errata\n");
                            memset(buffer, 0, sizeof(buffer));
                            strcpy(buffer, "Data errrata");
                        }
                        if ((send(new_socket, buffer, sizeof(buffer), 0)) < 0)
                        {
                            close(new_socket);
                            printf("Connection closed\n");
                            exit(-1);
                        }
                    }
                    break;
                case 2: //BOOK DA DATA A DATA
                    if ((recv(new_socket, buffer, sizeof(buffer), 0)) < 0)
                    {
                        close(new_socket);
                        printf("Connection closed\n");
                        exit(-1);
                    }
                    token = strtok(buffer, del);     //BOOK
                    temp1 = atoi(strtok(NULL, del)); //Num ombrellone
                    dateS = atoi(strtok(NULL, del)); //Data start
                    dateE = atoi(strtok(NULL, del)); //Data end
                    pthread_mutex_lock(&block_ombrellone[temp1 - 1]);
                    temp = addDate(&(ombrelloni[temp1 - 1].prenotazioni), &ombrelloni[temp1 - 1].numPrenotazioni, dateS, dateE);
                    printOmbrellone(ombrelloni[temp1 - 1].prenotazioni, temp1);
                    pthread_mutex_unlock(&block_ombrellone[temp1 - 1]);
                    if (temp != 0)
                    {
                        pthread_mutex_lock(&block_log);
                        saveLog(BOOK, temp1, dateS, dateE);
                        pthread_mutex_unlock(&block_log);
                        printf("Prenotazione effettuata\n");
                        memset(buffer, 0, sizeof(buffer));
                        strcpy(buffer, "Prenotazione effettuata");
                    }
                    else
                    {
                        printf("Data errata\n");
                        memset(buffer, 0, sizeof(buffer));
                        strcpy(buffer, "Data errrata");
                    }
                    if ((send(new_socket, buffer, sizeof(buffer), 0)) < 0)
                    {
                        close(new_socket);
                        printf("Connection closed\n");
                        exit(-1);
                    }
                    printf("\n################################################################\n");
                    break;
                default:
                    break;
                }
            }
            else
            {
                strcpy(buffer, "NAVAILABLE");
                if ((send(new_socket, buffer, sizeof(buffer), 0)) < 0)
                {
                    close(new_socket);
                    printf("Connection closed\n");
                    exit(-1);
                }
            }
        }
    }
    else if (strcmp(buffer, "CANCEL") == 0)
    {
        if ((recv(new_socket, buffer, sizeof(buffer), 0)) < 0)
        {
            close(new_socket);
            printf("Connection closed\n");
            exit(-1);
        }
        fprintf(stderr, "Client: %s\n", buffer);
        token = strtok(buffer, del); //String: CANCEL
        token = strtok(NULL, del);   //Num ombrellone
        temp1 = atoi(token);
        fprintf(stderr, "Num ombrellone: %d\n", temp1);
        if (isFull(temp1 - 1) == 0)
        {
            strcpy(buffer, "AVAILABLE");
            if ((send(new_socket, buffer, sizeof(buffer), 0)) < 0)
            {
                close(new_socket);
                printf("Connection closed\n");
                exit(-1);
            }
            if ((recv(new_socket, buffer, sizeof(buffer), 0)) < 0)
            {
                close(new_socket);
                printf("Connection closed\n");
                exit(-1);
            }
            token = strtok(buffer, del);     //CANCEL
            temp1 = atoi(strtok(NULL, del)); //Num ombrellone
            dateS = atoi(strtok(NULL, del)); //Data start
            dateE = atoi(strtok(NULL, del)); //Data end
            temp = seekPrenotazione(ombrelloni[temp1 - 1].prenotazioni, dateS, dateE);
            if (temp != -1)
            {
                deletePrenotazione(&(ombrelloni[temp1 - 1].prenotazioni), dateS, dateE);
                ombrelloni[temp1 - 1].numPrenotazioni--;
                pthread_mutex_lock(&block_log);
                saveLog(CANCEL, temp, dateS, dateE);
                pthread_mutex_unlock(&block_log);
                printf("Cancellazione effettuata\n");
                memset(buffer, 0, sizeof(buffer));
                strcpy(buffer, "CANCEL OK");
                printOmbrellone(ombrelloni[temp1 - 1].prenotazioni, temp1);
                if ((send(new_socket, buffer, sizeof(buffer), 0)) < 0)
                {
                    close(new_socket);
                    printf("Connection closed\n");
                    exit(-1);
                }
            }
            else
            {
                printf("Cancellazione fallita, prenotazione inesistente\n");
                memset(buffer, 0, sizeof(buffer));
                strcpy(buffer, "Prenotazione inesistente");
                printOmbrellone(ombrelloni[temp1 - 1].prenotazioni, temp1);
                if ((send(new_socket, buffer, sizeof(buffer), 0)) < 0)
                {
                    close(new_socket);
                    printf("Connection closed\n");
                    exit(-1);
                }
            }
        }
        else
        {
            strcpy(buffer, "NAVAILABLE");
            if ((send(new_socket, buffer, sizeof(buffer), 0)) < 0)
            {
                close(new_socket);
                printf("Connection closed\n");
                exit(-1);
            }
        }
    }
    else if (strcmp(buffer, "AVAILABLE") == 0)
    {
        recv(new_socket, buffer, sizeof(buffer), 0);
        printf("Buffer: %s\n", buffer);
        if (strcmp(buffer, "AVAILABLE") == 0)
        {
            if ((temp1 = getNumAvailable()) > 0)
            {
                strcpy(buffer, "AVAILABLE ");
                sprintf(buffer, "%s%d", buffer, temp1);
                send(new_socket, buffer, sizeof(buffer), 0);
            }
            else
            {
                strcpy(buffer, "NAVAILABLE");
                send(new_socket, buffer, sizeof(buffer), 0);
            }
        }
        else
        {
            token = strtok(buffer, del);
            token = strtok(NULL, del);
            temp1 = convertToInt(token);
            sendAvailableRow(temp1, new_socket);
        }
    }
    else
    {
        fprintf(stderr, "Client Quit\n");
    }
    close(new_socket);
}

void *main_task()
{
    while (1)
    {
        sleep(40);
        thpool_destroy(thpool);
        pthread_mutex_lock(&block_save);
        serverBusy = 1;
        //
        save();
        remove("log.txt");
        system("clear");
        thpool = thpool_init(5);
        fprintf(stderr, "Pid: %ld\n", (long)getpid());
        printOmbrelloni();
        serverBusy = 0;
        pthread_mutex_unlock(&block_save);
    }
}

/*
FILE
1   numOmbrellone fila numPrenotazioni
2   inizio fine
3   inizio fine
*/
void save()
{
    FILE *fp;
    int i, j;
    Prenotazione *current;

    if ((fp = fopen("prenotazioni.txt", "w")) == NULL)
    {
        printf("Errore nell'apertura");
        exit(-1);
    }
    for (i = 0; i < DIM; i++)
    {
        fprintf(fp, "%d %d %d\n", ombrelloni[i].numOmbrellone, ombrelloni[i].fila, ombrelloni[i].numPrenotazioni);
        current = ombrelloni[i].prenotazioni;
        for (j = 0; current && j < ombrelloni[i].numPrenotazioni; j++)
        {
            fprintf(fp, "%d %d\n", current->inizio, current->fine);
            current = current->next;
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void load()
{
    int i, j, inizio, fine, num, numP, fila;
    FILE *fp;
    if ((fp = fopen("prenotazioni.txt", "r")) == NULL)
    {
        printf("No, database. Creating a new one\n");
        return;
    }
    for (i = 0; i < DIM; i++)
    {
        fscanf(fp, "%d %d %d", &num, &fila, &numP);
        for (j = 0; j < numP; j++)
        {
            fscanf(fp, "%d %d", &inizio, &fine);
            addDate(&ombrelloni[i].prenotazioni, &ombrelloni[i].numPrenotazioni, inizio, fine);
            ombrelloni[i].fila = fila;
        }
    }
    fclose(fp);
    if ((fp = fopen("log.txt", "r")) != NULL)
    {
        updateDataByLog();
        remove("log.txt");
    }
}

int isDateCorrect(int giorno, int mese)
{
    switch (mese)
    {
    case 6:
        if (giorno >= 1 && giorno <= 30)
            return 1;
        else
            return 0;
    case 7:
        if (giorno >= 1 && giorno <= 31)
            return 1;
        else
            return 0;
    case 8:
        if (giorno >= 1 && giorno <= 31)
            return 1;
        else
            return 0;
    default:
        return 0;
    }
}

int convertInPeriod(int giorno, int mese)
{
    switch (mese)
    {
    case 6:
        return giorno;
    case 7:
        return 30 + giorno;
    case 8:
        return 61 + giorno;
    default:
        return -1;
    }
}

void convertInDate(int num)
{
    char string[31];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    if (num <= 30)
    {
        sprintf(string, "%d%s%d", num, "/06/", tm.tm_year + 1900);
        printf("%s", string);
    }
    else if (num <= 61)
    {
        sprintf(string, "%d%s%d", num - 30, "/07/", tm.tm_year + 1900);
        printf("%s", string);
    }
    else
    {
        sprintf(string, "%d%s%d", num - 61, "/08/", tm.tm_year + 1900);
        printf("%s", string);
    }
}

int daysLeft(int num)
{
    int periodo = 92;
    Prenotazione *current;
    pthread_mutex_lock(&block_ombrellone[num]);
    current = ombrelloni[num].prenotazioni;
    while (current != NULL)
    {
        periodo -= current->fine - (current->inizio - 1);
        current = current->next;
    }
    pthread_mutex_unlock(&block_ombrellone[num]);
    return periodo;
}

int isFull(int num)
{
    int n = daysLeft(num);
    if (n == 0)
        return 1; //E' pieno
    else
        return 0;
}

int areFull()
{
    int n, i;
    for (i = 0; i < DIM && isFull(i); i++)
        ;
    if (i == DIM - 1)
        return 0;
    else
        return 1;
}

int getNumAvailable()
{
    int n = 0, i;
    for (i = 0; i < DIM; i++)
    {
        if (isFull(i) == 0)
            n++; //Ci sono ancora ombrelloni disponibili
    }
    return n;
}

void sendAvailableRow(int row, int sock)
{
    int days, index = (row - 1) * 6, limit = index + 6, count = 0;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    for (; index < limit; index++)
    {
        if ((days = daysLeft(index)) > 0)
        {
            sprintf(buffer, "%d%s", ombrelloni[index].numOmbrellone, " ");
            send(sock, buffer, sizeof(buffer), 0);
        }
    }
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "FINE");
    send(sock, buffer, sizeof(buffer), 0);
}

void updateNumeriP(Prenotazione *head)
{
    Prenotazione *current = head;
    int n = 1;
    while (current != NULL)
    {
        current->numP = n++;
        current = current->next;
    }
}

int seekPrenotazione(Prenotazione *head, int inizio, int fine)
{
    //ritorna -1 se non c'è, altrimenti dà il numero della prenotazione
    Prenotazione *current = head;
    while (current != NULL)
    {
        if (current->inizio == inizio && current->fine == fine)
            return current->numP;
        current = current->next;
    }
    return -1;
}
void deletePrenotazione(Prenotazione **head, int inizio, int fine)
{
    // Store head node
    Prenotazione *temp = *head, *prev;
    if (temp != NULL && temp->inizio == inizio && temp->fine == fine)
    {
        *head = temp->next; // Changed head
        free(temp);         // free old head
        return;
    }
    while (temp != NULL && temp->inizio != inizio && temp->fine != fine)
    {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL)
        return;
    prev->next = temp->next;
    free(temp);
}
void addTop(Prenotazione **head, int inizio, int fine)
{
    Prenotazione *temp = (Prenotazione *)malloc(sizeof(Prenotazione));
    temp->inizio = inizio;
    temp->fine = fine;
    temp->numP = 1;
    temp->next = (*head);
    (*head) = temp;
}
void addBottom(Prenotazione **head, int inizio, int fine)
{
    Prenotazione *current = *head;
    Prenotazione *newNode = (Prenotazione *)malloc(sizeof(Prenotazione));
    newNode->inizio = inizio;
    newNode->fine = fine;
    newNode->numP = current->numP + 1;
    newNode->next = NULL;
    if (*head == NULL)
    {
        *head = newNode;
        return;
    }
    while (current->next != NULL)
        current = current->next;
    current->next = newNode;
    return;
}
void addAfter(Prenotazione **head, int inizio, int fine, int pos)
{
    Prenotazione *current = *head;
    Prenotazione *temp;
    while (current->numP != pos)
    {
        current = current->next;
    }
    temp = (Prenotazione *)malloc(sizeof(Prenotazione));
    temp->inizio = inizio;
    temp->fine = fine;
    temp->numP = pos + 1;
    temp->next = current->next;
    current->next = temp;
    current->next->next->numP += 1;
}

int addDate(Prenotazione **head, int *numPrenotazioni, int inizio, int fine)
{ //num = numero ombrellone -> vettore = numOmbrellone - 1
    Prenotazione *current = *head;

    if (current == NULL)
    {                               //nessuna prenotazione
        addTop(head, inizio, fine); //aggiunta in testa
        (*numPrenotazioni)++;
        updateNumeriP(*head);
        return 1;
    }
    else
    {
        do
        {
            if (current->inizio > fine)
            {
                addTop(head, inizio, fine);
                (*numPrenotazioni)++;
                updateNumeriP(*head);
                return 1;
            }
            else
            {
                if (current->next == NULL)
                {
                    if (current->fine >= inizio)
                    {
                        return 0;
                    }
                    else
                    {
                        addBottom(head, inizio, fine);
                        (*numPrenotazioni)++;
                        updateNumeriP(*head);
                        return 1;
                    }
                }
                else
                {
                    if (current->fine < inizio)
                    {
                        if (current->next->inizio > fine)
                        {
                            //inserisci il mezzo alle due prenotazioni
                            addAfter(head, inizio, fine, current->numP);
                            (*numPrenotazioni)++;
                            updateNumeriP(*head);
                            return 1;
                        }
                    }
                    else
                    {
                        return 0;
                    }
                }
            }
            current = current->next;
        } while (current != NULL);
    }
    return 0;
}

void initialize()
{
    int i, fila = 0;
    for (i = 0; i < DIM; i++)
    {
        //states[i] = 0;
        pthread_mutex_init(&block_ombrellone[i], NULL);
        ombrelloni[i].numOmbrellone = i + 1;
        if (i % 6 == 0)
            fila++;
        ombrelloni[i].fila = fila;
        ombrelloni[i].prenotazioni = NULL;
        ombrelloni[i].numPrenotazioni = 0;
    }
    pthread_mutex_init(&block_save, NULL);
    pthread_mutex_init(&block_log, NULL);
}

void printOmbrelloni()
{
    int i;
    Prenotazione *current;
    for (i = 0; i < DIM; i++)
    {
        printf("Ombrellone %d \tFila %d \tNumero Prenotazioni %d:\n", ombrelloni[i].numOmbrellone, ombrelloni[i].fila, ombrelloni[i].numPrenotazioni);
        current = ombrelloni[i].prenotazioni;
        if (current != NULL)
        {
            do
            {
                printf("Prenotazione %d \tData d'inizio: ", current->numP);
                convertInDate(current->inizio);
                printf("\tData di fine: ");
                convertInDate(current->fine);
                printf("\n");
                current = current->next;
            } while (current != NULL);
        }
        else
        {
            printf("Nessuna prenotazione\n");
        }
        printf("\n");
    }
}

void printOmbrellone(Prenotazione *head, int n)
{
    Prenotazione *current;
    printf("Ombrellone %d:\n", n);
    current = head;
    if (current != NULL)
    {
        do
        {
            printf("Prenotazione %d \tData d'inizio: ", current->numP);
            convertInDate(current->inizio);
            printf("\tData di fine: ");
            convertInDate(current->fine);
            printf("\n");
            current = current->next;
        } while (current != NULL);
    }
    else
    {
        printf("Nessuna prenotazione\n");
    }
}

int convertToInt(char s[])
{
    char s1[3];
    int i;
    memset(s1, 0, sizeof(s1));
    if(strlen(s) == 1)
        strncpy(s1, s, 1);
    else
        strncpy(s1, s, 2);
    for(i = 0; i < strlen(s1); i++)
        if(!isdigit(s1[i]))
            return -1;
    return atoi(s1);
}
void saveLog(int operation, int num, int inizio, int fine)
{
    FILE *fp;
    if ((fp = fopen("log.txt", "a")) == NULL)
    {
        printf("Error on opening\n");
        return;
    }
    fprintf(fp, "%d %d %d %d\n", operation, num, inizio, fine);
    fclose(fp);
}

void updateDataByLog()
{
    int operation, num, inizio, fine;
    FILE *fp;
    pthread_mutex_lock(&block_log);
    if ((fp = fopen("log.txt", "r")) == NULL)
    {
        printf("Error on opening\n");
        pthread_mutex_unlock(&block_log);
        return;
    }
    while ((fscanf(fp, "%d %d %d %d", &operation, &num, &inizio, &fine)) == 4)
    {
        if (operation == BOOK)
        {
            pthread_mutex_lock(&block_ombrellone[num - 1]);
            addDate(&ombrelloni[num - 1].prenotazioni, &ombrelloni[num - 1].numPrenotazioni, inizio, fine);
            pthread_mutex_unlock(&block_ombrellone[num - 1]);
        }
        else
        {
            pthread_mutex_lock(&block_ombrellone[num - 1]);
            deletePrenotazione(&ombrelloni[num - 1].prenotazioni, inizio, fine);
            ombrelloni[num - 1].numPrenotazioni--;
            pthread_mutex_unlock(&block_ombrellone[num - 1]);
        }
    }
    remove("log.txt");
    pthread_mutex_unlock(&block_log);
}

void signal_handler(int sig)
{
    if (sig == SIGINT) //gestione segnali da interruzione(ctrl+c)
    {
        printf("SIGINT\n");
        thpool_destroy(thpool);
        pthread_mutex_lock(&block_save);
        save();
        pthread_mutex_unlock(&block_save);
        close(server_fd);
        exit(0);
    }
    if (sig == SIGTERM)
    {
        printf("SIGTERM\n");
        thpool_destroy(thpool);
        pthread_mutex_lock(&block_save);
        save();
        pthread_mutex_unlock(&block_save);
        close(server_fd);
        exit(0);
    }
    if (sig == SIGQUIT)
    {
        printf("SIGQUIT\n");
        thpool_destroy(thpool);
        pthread_mutex_lock(&block_save);
        save();
        pthread_mutex_unlock(&block_save);
        close(server_fd);
        exit(0);
    }
    if (sig == SIGTSTP)
    {
        printf("SIGTSTP\n");
        thpool_destroy(thpool);
        pthread_mutex_lock(&block_save);
        save();
        pthread_mutex_unlock(&block_save);
        close(server_fd);
        exit(0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//funzioni per il client
int isDateCorrectC(int giorno, int mese)
{
    switch (mese)
    {
    case 6:
        if (giorno >= 1 && giorno <= 30)
            return 1;
        else
            return 0;
    case 7:
        if (giorno >= 1 && giorno <= 31)
            return 1;
        else
            return 0;
    case 8:
        if (giorno >= 1 && giorno <= 31)
            return 1;
        else
            return 0;
    default:
        return 0;
    }
}

int convertInPeriodC(int giorno, int mese)
{
    switch (mese)
    {
    case 6:
        return giorno;
    case 7:
        return 30 + giorno;
    case 8:
        return 61 + giorno;
    default:
        return -1;
    }
}

void convertInDateC(int num)
{
    char string[31];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    if (num <= 30)
    {
        sprintf(string, "%d%s%d", num, "/06/", tm.tm_year + 1900);
        printf("%s", string);
    }
    else if (num <= 61)
    {
        sprintf(string, "%d%s%d", num, "/06/", tm.tm_year + 1900);
        printf("%s", string);
    }
    else
    {
        sprintf(string, "%d%s%d", num, "/06/", tm.tm_year + 1900);
        printf("%s", string);
    }
}

int convertToIntC(char s[])
{
    char s1[3];
    int i;
    memset(s1, 0, sizeof(s1));
    if (strlen(s) == 1)
        strncpy(s1, s, 1);
    else
        strncpy(s1, s, 2);
    for (i = 0; i < strlen(s1); i++)
        if (!isdigit(s1[i]))
            return -1;
    return atoi(s1);
}

void book(int sock)
{
    char buffer[1024];
    int num, temp, day1, day2, mon1, mon2, ok = 0;
    time_t t;
    struct tm tm;
    if ((send(sock, "BOOK", sizeof(buffer), 0)) < 0)
    {
        close(sock);
        printf("Connection closed\n");
        exit(-1);
    }
    if ((recv(sock, buffer, sizeof(buffer), 0)) < 0)
    {
        close(sock);
        printf("Connection closed\n");
        exit(-1);
    }
    printf("Server: %s\n", buffer);
    if (strcmp("OK", buffer) == 0) //OK
    {
        printf("Prenotazione possibile!\n");
        do
        {
            printf("Scegli il numero dell'ombrellone da prenotare: \n");
            memset(buffer, 0, sizeof(buffer));
            scanf("%s", buffer);
           
            num = convertToInt(buffer);

            if (!(num >= 1 && num <= 30))
                printf("Input errato. Riprova!\n");
        } while (!(num >= 1 && num <= 30));

        system("clear");
        sprintf(buffer, "%s%d", "BOOK ", num);
        printf("Client: %s\n", buffer);
        if ((send(sock, buffer, sizeof(buffer), 0)) < 0)
        {
            close(sock);
            printf("Connection closed\n");
            exit(-1);
        }
        if ((recv(sock, buffer, sizeof(buffer), 0)) < 0)
        {
            close(sock);
            printf("Connection closed\n");
            exit(-1);
        }
        printf("Server: %s\n", buffer);
        if (strcmp("AVAILABLE", buffer) == 0) //Si procede con la prenotazione
        {
            printf("Scegli la modalità per prenotare: \n1. BOOK OMBRELLONE DATE \n2. BOOK OMBRELLONE DATE_START DATE_END \n3. CANCEL\n");
            do
            { //Controlli che la scelta fatta sia corretta
                memset(buffer, 0, sizeof(buffer));
                scanf("%s", buffer);
                temp = convertToInt(buffer);
                if (!(temp >= 1 && temp <= 3))
                    printf("Comando errato. Riprova!\n");
            } while (!(temp >= 1 && temp <= 3));
            system("clear");
            if ((send(sock, buffer, sizeof(buffer), 0)) < 0)
            {
                close(sock);
                printf("Connection closed\n");
                exit(-1);
            }
            switch (convertToInt(buffer))
            {
            case 1: //BOOK DA OGGI AD UNA DATA
                t = time(NULL);
                tm = *localtime(&t);
                day1 = tm.tm_mday;
                mon1 = tm.tm_mon + 1;

                if (isDateCorrect(tm.tm_mday, tm.tm_mon))
                {
                    strcpy(buffer, "CORRECT");
                    send(sock, buffer, sizeof(buffer), 1);
                    do //Input della data di fine con controlli
                    {
                        do
                        {
                            do
                            {
                                printf("Scegli il giorno di fine: \n");
                                memset(buffer, 0, sizeof(buffer));
                                scanf("%s", buffer);
                                if ((day2 = convertToInt(buffer)) < 0)
                                    printf("Input errato!\n");
                            } while (day2 < 0);
                            do
                            {
                                printf("Scegli il mese di fine: \n");
                                memset(buffer, 0, sizeof(buffer));
                                scanf("%s", buffer);
                                if ((mon2 = convertToInt(buffer)) < 0)
                                    printf("Input errato!\n");
                            } while (mon2 < 0);
                            if ((ok = isDateCorrect(day2, mon2)) == 0)
                                printf("Inserire una data corretta!\n");
                        } while (ok == 0);
                        if (ok = isAfter(day1, mon1, day2, mon2))
                            printf("Inserire una data valida!\n");
                    } while (ok);
                    system("clear");
                    day1 = convertInPeriod(day1, mon1);
                    day2 = convertInPeriod(day2, mon2);
                    sprintf(buffer, "%s%d%s%d%s%d", "BOOK ", num, " ", day1, " ", day2);
                    if ((send(sock, buffer, sizeof(buffer), 0)) < 0)
                    {
                        close(sock);
                        printf("Connection closed\n");
                        exit(-1);
                    }
                    if ((recv(sock, buffer, sizeof(buffer), 0)) < 0)
                    {
                        close(sock);
                        printf("Connection closed\n");
                        exit(-1);
                    }
                    printf("Server: %s\n", buffer);
                }
                else
                {
                    strcpy(buffer, "INCORRECT");
                    if ((send(sock, buffer, sizeof(buffer), 0)) < 0)
                    {
                        close(sock);
                        printf("Connection closed\n");
                        exit(-1);
                    }
                    fprintf(stderr, "Non puoi prenotare fuori dalla stagione\n");
                }
                break;
            case 2: //BOOK DA DATA A DATA
                do
                { //Input della data
                    do
                    { //Input della data di inizio con controlli
                        do
                        {
                            printf("Scegli il giorno di inizio: \n");
                            memset(buffer, 0, sizeof(buffer));
                            scanf("%s", buffer);
                            if ((day1 = convertToInt(buffer)) < 0)
                                printf("Input errato!\n");
                        } while (day1 < 0);
                        do
                        {
                            printf("Scegli il mese di inizio: \n");
                            memset(buffer, 0, sizeof(buffer));
                            scanf("%s", buffer);
                            if ((mon1 = convertToInt(buffer)) < 0)
                                printf("Input errato!\n");
                        } while (mon1 < 0);
                        printf("Data: %d %d\n", day1, mon1);
                        if ((ok = isDateCorrect(day1, mon1)) == 0)
                            printf("Ok: %d\tInserire una data corretta!\n", ok);
                    } while (ok == 0);
                    //
                    do //Input della data di fine
                    {
                        do
                        {
                            printf("Scegli il giorno di fine: \n");
                            memset(buffer, 0, sizeof(buffer));
                            scanf("%s", buffer);
                            if ((day2 = convertToInt(buffer)) < 0)
                                printf("Input errato!\n");
                        } while (day2 < 0);
                        do
                        {
                            printf("Scegli il mese di fine: \n");
                            memset(buffer, 0, sizeof(buffer));
                            scanf("%s", buffer);
                            if ((mon2 = convertToInt(buffer)) < 0)
                                printf("Input errato!\n");
                        } while (mon2 < 0);
                        if ((ok = isDateCorrect(day2, mon2)) == 0)
                            printf("Inserire una data corretta!\n");
                        //printf("Day1 %d Day2 %d Mon1 %d Mon2 %d\n", day1, day2, mon1, mon2);
                    } while (ok == 0);
                    printf("Day1: %d \tMon1: %d \tDay2: %d \tMon2: %d \nAfter: %d\n", day1, mon1, day2, mon2, isAfter(day1, mon1, day2, mon2));
                    if (ok = isAfter(day1, mon1, day2, mon2))
                        printf("Inserire una data valida!\n");
                } while (ok);
                //
                system("clear");
                printf("\n");
                day1 = convertInPeriod(day1, mon1);
                day2 = convertInPeriod(day2, mon2);
                sprintf(buffer, "%s%d%s%d%s%d", "BOOK ", num, " ", day1, " ", day2);
                if ((send(sock, buffer, sizeof(buffer), 0)) < 0)
                {
                    close(sock);
                    printf("Connection closed\n");
                    exit(-1);
                }
                if ((recv(sock, buffer, sizeof(buffer), 0)) < 0)
                {
                    close(sock);
                    printf("Connection closed\n");
                    exit(-1);
                }
                printf("Server: %s\n", buffer);
                break;
            default:
                break;
            }
        }
        else
        {
            if ((recv(sock, buffer, sizeof(buffer), 0)) < 0)
            {
                close(sock);
                printf("Connection closed\n");
                exit(-1);
            }
            printf("Server: %s\n", buffer);
            return;
        }
    }
    else if (strcmp("NOK", buffer) == 0) //NOK
    {
        printf("NOK \nIl server non è pronto!");
    }
    else if (strcmp("NAVAILABLE", buffer) == 0) //NAVAILABLE
    {
        printf("Tutti gli ombrelloni sono pieni!\n");
    }
}

void cancel(int sock)
{
    char buffer[1024];
    int num, temp, day1, day2, mon1, mon2, ok = 0;

    strcpy(buffer, "CANCEL");
    if ((send(sock, buffer, sizeof(buffer), 0)) < 0)
    {
        close(sock);
        printf("Connection closed\n");
        exit(-1);
    }
    printf("Scegli il numero dell'ombrellone da cui cancellare una prenotazione: \n");
    do
    {
        memset(buffer, 0, sizeof(buffer));
        scanf("%s", buffer);
        num = convertToInt(buffer);
        if (!(num >= 1 && num <= 30))
            printf("Input errato. Riprova!\n");
    } while (!(num >= 1 && num <= 30));
    system("clear");
    sprintf(buffer, "%s%d", "CANCEL ", num);
    printf("Client: %s\n", buffer);
    if ((send(sock, buffer, sizeof(buffer), 0)) < 0)
    {
        close(sock);
        printf("Connection closed\n");
        exit(-1);
    }
    if ((recv(sock, buffer, sizeof(buffer), 0)) < 0)
    {
        close(sock);
        printf("Connection closed\n");
        exit(-1);
    }
    printf("Server: %s\n", buffer);
    if (strcmp("AVAILABLE", buffer) == 0)
    {
        do
        {      //Input della data con controlli
            do //Input della data di inizio con controlli
            {
                do
                {
                    printf("Scegli il giorno di inizio: \n");
                    memset(buffer, 0, sizeof(buffer));
                    scanf("%s", buffer);
                    if ((day1 = convertToInt(buffer)) < 0)
                        printf("Input errato!\n");
                } while (day1 < 0);
                do
                {
                    printf("Scegli il mese di inizio: \n");
                    memset(buffer, 0, sizeof(buffer));
                    scanf("%s", buffer);
                    if ((mon1 = convertToInt(buffer)) < 0)
                        printf("Input errato!\n");
                } while (mon1 < 0);
                if ((ok = isDateCorrect(day1, mon1)) == 0)
                    printf("Inserire una data corretta!\n");
            } while (ok == 0);
            do //Input della data di fine con controlli
            {
                do
                {
                    printf("Scegli il giorno di fine: \n");
                    memset(buffer, 0, sizeof(buffer));
                    scanf("%s", buffer);
                    if ((day2 = convertToInt(buffer)) < 0)
                        printf("Input errato!\n");
                } while (day2 < 0);
                do
                {
                    printf("Scegli il mese di fine: \n");
                    memset(buffer, 0, sizeof(buffer));
                    scanf("%s", buffer);
                    if ((mon2 = convertToInt(buffer)) < 0)
                        printf("Input errato!\n");
                } while (mon2 < 0);
                if ((ok = isDateCorrect(day2, mon2)) == 0)
                    printf("Inserire una data corretta!\n");
            } while (ok == 0); //###########################################################################
            if (ok = isAfter(day1, mon1, day2, mon2))
                printf("Inserire una data valida!\n");
        } while (ok);
        //
        day1 = convertInPeriod(day1, mon1);
        day2 = convertInPeriod(day2, mon2);
        sprintf(buffer, "%s%d%s%d%s%d", "CANCEL ", num, " ", day1, " ", day2);
        if ((send(sock, buffer, sizeof(buffer), 0)) < 0)
        {
            close(sock);
            printf("Connection closed\n");
            exit(-1);
        }
        if ((recv(sock, buffer, sizeof(buffer), 0)) < 0)
        {
            close(sock);
            printf("Connection closed\n");
            exit(-1);
        }
        printf("Server: %s\n", buffer);
    }
    else
    {
        if ((recv(sock, buffer, sizeof(buffer), 0)) < 0)
        {
            close(sock);
            printf("Connection closed\n");
            exit(-1);
        }
        printf("Server: %s\n", buffer);
        return;
    }
}

void available(int sock)
{
    char buffer[1024], tmp[1024];
    int temp;
    strcpy(buffer, "AVAILABLE");
    if ((send(sock, buffer, sizeof(buffer), 0)) < 0)
    {
        close(sock);
        printf("Connection closed\n");
        exit(-1);
    }
    printf("1. AVAILABLE \n2. AVAILABLE $FILA\n");
    do
    { //Input comando e controlli
        memset(buffer, 0, sizeof(buffer));
        scanf("%s", buffer);
        temp = convertToInt(buffer);
        if (!(temp >= 1 && temp <= 2))
            printf("Input errato. Riprova!\n");
    } while (!(temp >= 1 && temp <= 2));
    system("clear");
    switch (convertToInt(buffer))
    {
    case 1:
        strcpy(buffer, "AVAILABLE");
        if ((send(sock, buffer, sizeof(buffer), 0)) < 0)
        {
            close(sock);
            printf("Connection closed\n");
            exit(-1);
        }
        if ((recv(sock, buffer, sizeof(buffer), 0)) < 0)
        {
            close(sock);
            printf("Connection closed\n");
            exit(-1);
        }
        fprintf(stderr, "%s\n", buffer);
        break;
    case 2:
        printf("Quale fila vuoi controllare?\n");
        do
        { //Input comando e controlli
            memset(buffer, 0, sizeof(buffer));
            scanf("%s", buffer);
            temp = convertToInt(buffer);
            if (!(temp >= 1 && temp <= 5))
                printf("Input errato. Riprova!\n");
        } while (!(temp >= 1 && temp <= 5));
        system("clear");
        sprintf(buffer, "%s%d", "AVAILABLE ", temp);
        if ((send(sock, buffer, sizeof(buffer), 0)) < 0)
        {
            close(sock);
            printf("Connection closed\n");
            exit(-1);
        }
        printf("Ombrelloni disponibili : ");
        while (1)
        {
            if ((recv(sock, buffer, sizeof(buffer), 0)) < 0)
            {
                close(sock);
                printf("Connection closed\n");
                exit(-1);
            }
            if (strcmp(buffer, "FINE") == 0)
                break;
            else
                printf("%s ", buffer);
        }
        printf("\n");
        if ((recv(sock, buffer, sizeof(buffer), 0)) < 0)
        {
            close(sock);
            printf("Connection closed\n");
            exit(-1);
        }
        temp = atoi(buffer);
        break;
    default:
        break;
    }
}

int isAfter(int day1, int mon1, int day2, int mon2)
{
    if (mon1 == mon2)
    {
        if (day1 > day2)
            return 1;
        else
            return 0;
    }
    else if (mon1 < mon2)
        return 0;
    else
        return 1;
}