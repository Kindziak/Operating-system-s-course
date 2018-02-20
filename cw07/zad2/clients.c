#include "clients.h"

int FIFOidentifier;
FIFOqueue * fifo = NULL;
sem_t * SEMAPHOREchairs;
sem_t * SEMAPHOREbarber;
sem_t * SEMAPHOREclients;
int numberOfDoneClients = 0;

int main(int argc, char ** argv) {
  int numberOfClients, numberOfCuts;

  signal(SIGTSTP, signalHandler);
  signal(SIGRTMIN, signalHandler);
  if (argc != 3) errorHandler(1);
  if (((numberOfClients = atoi(argv[1])) <= 0) || ((numberOfCuts = atoi(argv[2])) <= 0)) errorHandler(2);

  createWaitingRoomAsFIFO();
  createSemaphore();
  goToCut(numberOfClients,numberOfCuts);
  return 0;
}

void goToCut(int numberOfClients, int numberOfCuts) {
  int isEnd = 0;
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
  int wasCut;

  for(int i = 1 ; i <= numberOfClients ; i++) {
    pid_t client = fork();
    if (client < 0) errorHandler(3);
    else if (client == 0) {
      while (numberOfCuts > numberOfDoneClients) {
        if ((sem_wait(SEMAPHOREclients)) == -1) errorHandler(14);
        if ((sem_wait(SEMAPHOREchairs)) == -1) errorHandler(14);

        sem_getvalue(SEMAPHOREbarber, &state);
        if (state == 0) {
          if ((sem_post(SEMAPHOREbarber)) == -1) errorHandler(14);
          printf("Budzenie golibrody. Czas: %ld, Golibroda zaczyna strzyc %d.\n", getTime(), getpid());
          if ((sem_post(SEMAPHOREbarber)) == -1) errorHandler(14);
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
        if ((sem_post(SEMAPHOREchairs)) == -1) errorHandler(14);
        if ((sem_post(SEMAPHOREclients)) == -1) errorHandler(14);
        if (wasCut != -1) {
          sigsuspend(&mask);
          signal(SIGTSTP, signalHandler);
          signal(SIGRTMIN, signalHandler);
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
  if ((SEMAPHOREclients = sem_open("/SEMAPHOREclients",O_RDWR)) == SEM_FAILED) errorHandler(8);
  if ((SEMAPHOREchairs = sem_open("/SEMAPHOREchairs",O_RDWR)) == SEM_FAILED) errorHandler(8);
  if ((SEMAPHOREbarber = sem_open("/SEMAPHOREbarber",O_RDWR)) == SEM_FAILED) errorHandler(8);
}

void createWaitingRoomAsFIFO() {
  if ((FIFOidentifier = shm_open("/FIFOidentifier", O_RDWR,0600)) == -1) errorHandler(6);
  attachFIFO();
}

void attachFIFO() {
  void * fifoAsVoid;

  if ((fifoAsVoid = mmap(NULL, sizeof(FIFOqueue),PROT_WRITE | PROT_READ, MAP_SHARED, FIFOidentifier,0)) == (void *)(-1)) errorHandler(7);
  else fifo = (FIFOqueue *)fifoAsVoid;
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
  sem_close(SEMAPHOREbarber);
  sem_close(SEMAPHOREchairs);
  sem_close(SEMAPHOREclients);
  munmap(fifo, sizeof(FIFOqueue));
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
    case 14: printf("ERROR! Niepoprawne wykonanie polecenia: semctl!\n"); deleteSemaphoreAndFIFO(); exit(9);
    case 10: printf("ERROR! Niepoprawne wykonanie polecenia: shmdt!\n"); deleteSemaphoreAndFIFO(); exit(10);
    case 12: printf("ERROR! Niepoprawne wykonanie polecenia: semop!\n"); deleteSemaphoreAndFIFO(); exit(12);
    case 13: printf("ERROR! Niepoprawne wykonanie polecenia: clock_gettime!\n"); deleteSemaphoreAndFIFO(); exit(13);
    default: printf("Brak takiego bledu!\n"); exit(0);
  }
}
