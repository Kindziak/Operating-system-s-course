#include "sleepingBarber.h"

int FIFOidentifier;
FIFOqueue * fifo = NULL;
sem_t * SEMAPHOREchairs;
sem_t * SEMAPHOREbarber;
sem_t * SEMAPHOREclients;

int main(int argc, char ** argv) {
  int numberOfChairs;

  if (signal(SIGTSTP, signalHandler) == SIG_ERR) errorHandler(1);
  if (argc != 2) errorHandler(2);
  if ((numberOfChairs = atoi(argv[1])) <= 0) errorHandler(3);

  createWaitingRoomAsFIFO(numberOfChairs);
  createSemaphore();
  workAndSleep();

  return 0;
}

void workAndSleep() {
  for (;;) {
    pid_t clientToCut;
    struct sembuf operation;

    if ((sem_wait(SEMAPHOREbarber)) == -1) errorHandler(14);
    if ((sem_wait(SEMAPHOREchairs)) == -1) errorHandler(14);

    printf("Budzenie golibrody.\n");
    clientToCut = fifo -> clientToCut;

    sem_post(SEMAPHOREchairs);
    cut(clientToCut);

    for (;;) {
      if ((sem_wait(SEMAPHOREchairs)) == -1) errorHandler(14);
      clientToCut = popFIFO(fifo);

      if (clientToCut == -1) {
        printf("Czas: %ld, Golibroda idzie spac.\n", getTime());
        if ((sem_wait(SEMAPHOREbarber)) == -1) errorHandler(14);
        if ((sem_post(SEMAPHOREchairs)) == -1) errorHandler(14);
        break;
      }
      else {
        if ((sem_post(SEMAPHOREchairs)) == -1) errorHandler(14);
        cut(clientToCut);
      }
    }
  }
}

long getTime() {
  struct timespec currentTime;
  if (clock_gettime(CLOCK_MONOTONIC, &currentTime) == -1) errorHandler(13);
  return currentTime.tv_nsec / 1000;
}

void cut(pid_t clientToCut) {
  printf("Czas: %ld, Golibroda zaczyna strzyc %d.\n", getTime(), clientToCut);
  kill(clientToCut, SIGRTMIN);
  printf("Czas: %ld, Golibroda konczy strzyc %d.\n", getTime(), clientToCut);
}

void createSemaphore() {
  if ((SEMAPHOREclients = sem_open("/SEMAPHOREclients",O_RDWR | O_CREAT | O_EXCL , 0600,1)) == SEM_FAILED) errorHandler(8);
  if ((SEMAPHOREchairs = sem_open("/SEMAPHOREchairs",O_RDWR | O_CREAT | O_EXCL , 0600,1)) == SEM_FAILED) errorHandler(8);
  if ((SEMAPHOREbarber = sem_open("/SEMAPHOREbarber",O_RDWR | O_CREAT | O_EXCL , 0600,0)) == SEM_FAILED) errorHandler(8);
}

void createWaitingRoomAsFIFO(int numberOfChairs) {
  if ((FIFOidentifier = shm_open("/FIFOidentifier", O_CREAT | O_RDWR | O_TRUNC ,0600)) == -1) errorHandler(6);
  attachFIFO();
  initializeFIFO(fifo, numberOfChairs);
}

void attachFIFO() {
  void * fifoAsVoid;

  ftruncate(FIFOidentifier, sizeof(FIFOqueue));
  if ((fifoAsVoid = mmap(NULL, sizeof(FIFOqueue),PROT_WRITE | PROT_READ, MAP_SHARED, FIFOidentifier,0)) == (void *)(-1)) errorHandler(7);
  else fifo = (FIFOqueue *)fifoAsVoid;
}

void deleteSemaphoreAndFIFO() {
  shm_unlink("/FIFOidentifier");
  munmap(fifo, sizeof(FIFOqueue));
  sem_close(SEMAPHOREchairs);
  sem_unlink("/SEMAPHOREchairs");
  sem_close(SEMAPHOREclients);
  sem_unlink("/SEMAPHOREclients");
  sem_close(SEMAPHOREbarber);
  sem_unlink("/SEMAPHOREbarber");
}

void signalHandler(int signalType) {
  switch (signalType) {
    case SIGTSTP:
      deleteSemaphoreAndFIFO();
      exit(0);
    default: break;
  }
}

void errorHandler(int number) {
  switch (number) {
    case 1: printf("ERROR! Niepoprawne wykonanie polecenia: signal!\n"); deleteSemaphoreAndFIFO(); exit(1);
    case 2: printf("ERROR! Niepoprawna ilosc argumentow! Powinien byc jeden: ilosc krzesel w poczekalni.\n"); exit(2);
    case 3: printf("ERROR! Niepoprawny argument! Ilosc krzesel w poczakalni musi byc wieksza od 0.\n"); exit(3);
    case 4: printf("ERROR! Niepoprawne wykonanie polecenia: getenv!\n"); deleteSemaphoreAndFIFO(); exit(4);
    case 5: printf("ERROR! Niepoprawne wykonanie polecenia: ftok!\n"); deleteSemaphoreAndFIFO(); exit(5);
    case 6: printf("ERROR! Niepoprawne wykonanie polecenia: shmget!\n"); deleteSemaphoreAndFIFO(); exit(6);
    case 7: printf("ERROR! Niepoprawne wykonanie polecenia: shmat!\n"); deleteSemaphoreAndFIFO(); exit(7);
    case 8: printf("ERROR! Niepoprawne wykonanie polecenia: semget!\n"); deleteSemaphoreAndFIFO(); exit(8);
    case 9: printf("ERROR! Niepoprawne wykonanie polecenia: semctl!\n"); deleteSemaphoreAndFIFO(); exit(9);
    case 10: printf("ERROR! Niepoprawne wykonanie polecenia: shmdt!\n"); deleteSemaphoreAndFIFO(); exit(10);
    case 11: printf("ERROR! Niepoprawne wykonanie polecenia: shmctl!\n"); deleteSemaphoreAndFIFO(); exit(11);
    case 12: printf("ERROR! Niepoprawne wykonanie polecenia: semop!\n"); deleteSemaphoreAndFIFO(); exit(12);
    case 13: printf("ERROR! Niepoprawne wykonanie polecenia: clock_gettime!\n"); deleteSemaphoreAndFIFO(); exit(13);
    default: printf("Brak takiego bledu!\n"); exit(0);
  }
}
