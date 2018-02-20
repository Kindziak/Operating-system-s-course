#include "clients.h"

key_t keyForFIFO;
int FIFOidentifier, SEMAPHOREidentifier;
FIFOqueue * fifo = NULL;
int numberOfDoneClients = 0;
int wasCut;

int main(int argc, char ** argv) {
  int numberOfClients, numberOfCuts;
  int isEnd = 0;

  signal(SIGTSTP, signalHandler);
  signal(SIGRTMIN, signalHandler);
  if (argc != 3) errorHandler(1);
  if (((numberOfClients = atoi(argv[1])) <= 0) || ((numberOfCuts = atoi(argv[2])) <= 0)) errorHandler(2);

  createWaitingRoomAsFIFO();
  createSemaphore();
  goToCut(numberOfClients,numberOfCuts,isEnd);
  return 0;
}

void goToCut(int numberOfClients, int numberOfCuts,int isEnd) {
  sigset_t mask, maskmask;

  sigfillset(&mask);
  sigdelset(&mask,SIGRTMIN);
  sigdelset(&mask,SIGTSTP);
  sigemptyset(&maskmask);
  sigaddset(&maskmask,SIGRTMIN);
  sigprocmask(SIG_BLOCK,&maskmask,NULL);

  cutting(numberOfClients,mask,numberOfCuts);
  while(isEnd == 0) {
    wait(NULL);
    if (ECHILD == errno) isEnd = 1;
  }
  printf("Konies strzyzenia.\n");
}

void cutting(int numberOfClients, sigset_t mask, int numberOfCuts) {
  int state;
  pid_t clientPID;
  struct sembuf operation;

  for(int i = 1 ; i <= numberOfClients ; i++) {
    pid_t client = fork();
    if (client < 0) errorHandler(3);
    else if (client == 0) {
      while (numberOfCuts > numberOfDoneClients) {
        operation.sem_num = 2;
        operation.sem_op = -1;
        operation.sem_flg = 0;
        if (semop(SEMAPHOREidentifier, &operation, 1) == -1) errorHandler(12);
        operation.sem_num = 1;
        operation.sem_op = -1;
        operation.sem_flg = 0;
        if (semop(SEMAPHOREidentifier, &operation, 1) == -1) errorHandler(12);

        if ((state = semctl(SEMAPHOREidentifier,0,GETVAL)) == -1) errorHandler(9);
        else if (state == 0) {
          operation.sem_num = 0;
          operation.sem_op = 1;
          operation.sem_flg = 0;
          if (semop(SEMAPHOREidentifier, &operation, 1) == -1) errorHandler(12);

          printf("Budzenie golibrody. Czas: %ld, Golibroda zaczyna strzyc %d.\n", getTime(), getpid());

          operation.sem_num = 0;
          operation.sem_op = 1;
          operation.sem_flg = 0;
          if (semop(SEMAPHOREidentifier, &operation, 1) == -1) errorHandler(12);

          fifo -> clientToCut = getpid();
          wasCut = 1;
        }
        else {
          int nextClient;
          if((nextClient = pushFIFO(fifo,getpid())) == -1) {
            printf("Nie ma miejsca w kolejce.\n");
            wasCut = -1;
          }
          else {
            printf("Zajecie kolejki.\n");
            wasCut = 0;
          }
        }
        operation.sem_num = 1;
        operation.sem_op = 1;
        operation.sem_flg = 0;
        if (semop(SEMAPHOREidentifier, &operation, 1) == -1) errorHandler(12);
        operation.sem_num = 2;
        operation.sem_op = 1;
        operation.sem_flg = 0;
        if (semop(SEMAPHOREidentifier, &operation, 1) == -1) errorHandler(12);

        if (wasCut != -1) {
          sigsuspend(&mask);
          printf("Czas: %ld, Klient wychodzi %d.\n", getTime(), getpid());
        }
      }
      exit(0);
    }
  }
}

long getTime() {
  struct timespec currentTime;
  if (clock_gettime(CLOCK_MONOTONIC, &currentTime) == -1) errorHandler(13);
  return currentTime.tv_nsec / 1000;
}

void createSemaphore() {
  if ((SEMAPHOREidentifier = semget(keyForFIFO, 0, 0)) == -1) errorHandler(8);
}

void createWaitingRoomAsFIFO() {
  createKey();
  if ((FIFOidentifier = shmget(keyForFIFO, 0, 0)) == -1) errorHandler(6);
  attachFIFO();
}

void attachFIFO() {
  void * fifoAsVoid;

  if ((fifoAsVoid = shmat(FIFOidentifier, NULL, 0)) == (void *)(-1)) errorHandler(7);
  else fifo = (FIFOqueue*)fifoAsVoid;
}

void createKey() {
  char * path;

  if ((path = getenv(FIFO_PATH)) == NULL) errorHandler(4);
  if ((keyForFIFO = ftok(path, KEY_GENERATE_NUMBER)) == -1) errorHandler(5);
}

void signalHandler(int signalType) {
  switch (signalType) {
    case SIGTSTP:
      exit(0);
      break;
    default: break;
  }
  if (signalType == SIGRTMIN) numberOfDoneClients++;
}

void deleteSemaphoreAndFIFO() {
  if (shmdt(fifo) == -1) errorHandler(10);
}

void errorHandler(int number) {
  switch (number) {
    case 1: printf("ERROR! Niepoprawna ilosc argumentow! Powinny byc dwa: ilos klientow do stworzenia i ilosc strzyzen kazdego klienta.\n"); exit(1);
    case 2: printf("ERROR! Niepoprawne argumenty! Ilosc klientow do stworzenia i ilosc strzyzen kazdego klienta powinna byc wieksza od 0.\n"); exit(2);
    case 3: printf("ERROR! Niepoprawne uzycie fork!\n"); deleteSemaphoreAndFIFO(); exit(3);
    case 4: printf("ERROR! Niepoprawne wykonanie polecenia: getenv!\n"); deleteSemaphoreAndFIFO(); exit(4);
    case 5: printf("ERROR! Niepoprawne wykonanie polecenia: ftok!\n"); deleteSemaphoreAndFIFO(); exit(5);
    case 6: printf("ERROR! Niepoprawne wykonanie polecenia: shmget!\n"); deleteSemaphoreAndFIFO(); exit(6);
    case 7: printf("ERROR! Niepoprawne wykonanie polecenia: shmat!\n"); deleteSemaphoreAndFIFO(); exit(7);
    case 8: printf("ERROR! Niepoprawne wykonanie polecenia: semget!\n"); deleteSemaphoreAndFIFO(); exit(8);
    case 9: printf("ERROR! Niepoprawne wykonanie polecenia: semctl!\n"); deleteSemaphoreAndFIFO(); exit(9);
    case 10: printf("ERROR! Niepoprawne wykonanie polecenia: shmdt!\n"); deleteSemaphoreAndFIFO(); exit(10);
    case 12: printf("ERROR! Niepoprawne wykonanie polecenia: semop!\n"); deleteSemaphoreAndFIFO(); exit(12);
    case 13: printf("ERROR! Niepoprawne wykonanie polecenia: clock_gettime!\n"); deleteSemaphoreAndFIFO(); exit(13);
    default: printf("Brak takiego bledu!\n"); exit(0);
  }
}
