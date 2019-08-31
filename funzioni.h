#define PORT 8080
#define DIM 30 //numero di ombrelloni. Organizzati in 5 file da 6 ombrelloni (5x6)
#define BOOK 1
#define CANCEL 2
#define debug 1

//strutture
typedef struct Prenotazione //nodo
{
    int inizio, fine, numP; //numero prenotazione
    struct Prenotazione *next; //elemento successivo
} Prenotazione;

typedef struct ombrellone
{
    int numOmbrellone, fila, numPrenotazioni;
    Prenotazione *prenotazioni;  //head della lista
} Ombrellone;

//variabili globali
Ombrellone ombrelloni[DIM];
int server_fd, serverBusy;
threadpool master;
threadpool thpool;
pthread_mutex_t block_ombrellone[DIM];
pthread_mutex_t block_log;
pthread_mutex_t block_save;

//funzioni per il server
void* manage(int* sock);
void* main_task();
int isDateCorrect(int giorno, int mese);        //Controlla che la data stia nel range del periodo estivo Giugno - Agosto
int convertInPeriod(int giorno, int mese);        //Converte la data in un numero da 1-92 (giorni totali dei mesi da Giugno ad Agosto)
void convertInDate(int num);                            //Converte la data in numero 1-92 in una data nel formato gg/mm/aaaa e la stampa
int daysLeft(int num);  //numero di giorni liberi di un ombrellone
int isFull(int num);    //Return 1 se è pieno, 0 se libero
int areFull();
int getNumAvailable();
void sendAvailableRow(int row, int sock);
void updateNumeriP(Prenotazione* head);
int seekPrenotazione(Prenotazione* head, int inizio, int fine);
void deletePrenotazione(Prenotazione** head, int inizio, int fine);
void addTop(Prenotazione** head, int inizio, int fine);
void addBottom(Prenotazione** head, int inizio, int fine);
void addAfter(Prenotazione** head, int inizio, int fine, int pos);
int addDate(Prenotazione** head, int* numPrenotazioni, int inizio, int fine);
void initialize();
void printOmbrelloni();
void printOmbrellone(Prenotazione* head, int n);
int convertToInt(char s[]);
void saveLog(int operation, int num, int inizio, int fine);
void save();
void load();
void updateDataByLog();
void signal_handler(int sig);


//funzioni per il client
int isDateCorrectC(int giorno, int mese);        //Controlla che la data stia nel range del periodo estivo Giugno - Agosto
int isTodayDateCorrect();
int convertInPeriodC(int giorno, int mese);        //Converte la data in un numero da 1-92 (giorni totali dei mesi da Giugno ad Agosto)
void convertInDateC(int num);                            //Converte la data in numero 1-92 in una data nel formato gg/mm/aaaa e la stampa
int convertToIntC(char s[]);
void book(int sock);
void cancel(int sock);
void available(int sock);
int isAfter(int day1, int mon1, int day2, int mon2);