#include "sleepingBarber.h"

key_t keyForFIFO;
int FIFOidentifier, SEMAPHOREidentifier;
FIFOqueue * fifo = NULL;

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

    operation.sem_num = 0;
    operation.sem_op = -1;
    operation.sem_flg = 0;
    if (semop(SEMAPHOREidentifier, &operation, 1) == -1) errorHandler(12);
    operation.sem_num = 1;
    operation.sem_op = -1;
    operation.sem_flg = 0;
    if (semop(SEMAPHOREidentifier, &operation, 1) == -1) errorHandler(12);

    clientToCut = fifo -> clientToCut;

    operation.sem_num = 1;
    operation.sem_op = 1;
    operation.sem_flg = 0;
    if (semop(SEMAPHOREidentifier, &operation, 1) == -1) errorHandler(12);

    cut(clientToCut);

    for (;;) {
      operation.sem_num = 1;
      operation.sem_op = -1;
      operation.sem_flg = 0;
      if (semop(SEMAPHOREidentifier, &operation, 1) == -1) errorHandler(12);

      clientToCut = popFIFO(fifo);

      if (clientToCut != -1) {
        operation.sem_num = 1;
        operation.sem_op = 1;
        operation.sem_flg = 0;
        if (semop(SEMAPHOREidentifier, &operation, 1) == -1) errorHandler(12);

        cut(clientToCut);
      }
      else {
        printf("Czas: %ld, Golibroda idzie spac.\n", getTime());

        operation.sem_num = 0;
        operation.sem_op = -1;
        operation.sem_flg = 0;
        if (semop(SEMAPHOREidentifier, &operation, 1) == -1) errorHandler(12);
        operation.sem_num = 1;
        operation.sem_op = 1;
        operation.sem_flg = 0;
        if (semop(SEMAPHOREidentifier, &operation, 1) == -1) errorHandler(12);

        break;
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
  if ((SEMAPHOREidentifier = semget(keyForFIFO, AMOUNT_OF_SEMAPHORES, IPC_CREAT | 0600)) == -1) errorHandler(8);

  if (semctl(SEMAPHOREidentifier, 0, SETVAL, 0) == -1) errorHandler(9);
  for (int i = 1 ; i < AMOUNT_OF_SEMAPHORES -1 ; i++) {
    if (semctl(SEMAPHOREidentifier, i, SETVAL, 1) == -1) errorHandler(9);
  }
}

void createWaitingRoomAsFIFO(int numberOfChairs) {
  createKey();
  if ((FIFOidentifier = shmget(keyForFIFO, sizeof(FIFOqueue), IPC_CREAT | 0600)) == -1) errorHandler(6);
  attachFIFO();
  initializeFIFO(fifo, numberOfChairs);
}

void attachFIFO() {
  void * fifoAsVoid;

  if ((fifoAsVoid = shmat(FIFOidentifier, NULL, 0)) == (void *)(-1)) errorHandler(7);
  else fifo = (FIFOqueue *)fifoAsVoid;
}

void createKey() {
  char * path;

  if ((path = getenv(FIFO_PATH)) == NULL) errorHandler(4);
  if ((keyForFIFO = ftok(path, KEY_GENERATE_NUMBER)) == -1) errorHandler(5);
}

void deleteSemaphoreAndFIFO() {
  if (shmdt(fifo) == -1) errorHandler(10);
  if (shmctl(FIFOidentifier, IPC_RMID, NULL) == -1) errorHandler(11);
  if (semctl(SEMAPHOREidentifier, 0, IPC_RMID) == -1) errorHandler(9);
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
