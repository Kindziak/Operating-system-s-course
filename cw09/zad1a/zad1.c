#include "zad1.h"

sem_t * semaphoreReaders;
sem_t * semaphoreWriters;
int commonData[DATA_SIZE];
int workingReaders = 0;

int main(int argc, char ** argv) {
  srand(time(NULL));
  ArgumentsForThread * arguments = calloc(1,sizeof(ArgumentsForThread));
  arguments = initializeArguments(argc,argv,arguments);
  prepareData();
  createSemaphore();
  arguments = createThreads(arguments);
  waitForEndingThreads(arguments);
  deleteArguments(arguments);

  return 0;
}

ArgumentsForThread * initializeArguments(int argc, char ** argv, ArgumentsForThread * arguments) {
  switch (argc) {
    case 3:
      arguments -> raport = 0;
      break;
    case 4:
      if (strcmp(argv[3],"-i") != 0) errorHandler(3);
      arguments -> raport = 1;
      break;
    default: errorHandler(1);
  }
  if ((arguments -> numberOfReaders = atoi(argv[1])) <= 0 || (arguments -> numberOfWriters = atoi(argv[2])) <= 0) errorHandler(2);
  arguments -> threadsForReaders = calloc(arguments -> numberOfReaders,sizeof(pthread_t));
  arguments -> threadsForWriters = calloc(arguments -> numberOfWriters,sizeof(pthread_t));
  return arguments;
}

void createSemaphore() {
  semaphoreReaders = malloc(sizeof(sem_t));
  semaphoreWriters = malloc(sizeof(sem_t));
  if (sem_init(semaphoreReaders,0,1) == -1) errorHandler(4);
  if (sem_init(semaphoreWriters,0,1) == -1) errorHandler(4);
}

void prepareData() {
  for (int i = 0 ; i < DATA_SIZE ; i++) commonData[i] = i;
}
ArgumentsForThread * createThreads(ArgumentsForThread * arguments) {
  for (int i = 0 ; i < arguments -> numberOfReaders ; i++) {
    if (pthread_create(&(arguments -> threadsForReaders[i]),NULL,readersHandler,(void *)arguments) != 0) errorHandler(5);
  }
  for (int i = 0 ; i < arguments -> numberOfWriters ; i++) {
    if (pthread_create(&(arguments -> threadsForWriters[i]),NULL,writersHandler,(void *)arguments) != 0) errorHandler(5);
  }
  return arguments;
}

void * readersHandler(void * args) {
  ArgumentsForThread * arguments = (ArgumentsForThread*)args;
  //for (;;) {
    if (sem_wait(semaphoreReaders) != 0) errorHandler(9);
    workingReaders++;
    if (workingReaders == 1) {
      if (sem_wait(semaphoreWriters) != 0) errorHandler(9);
    }
    if (sem_post(semaphoreReaders) != 0) errorHandler(8);
    readData(arguments -> raport);
    //usleep(1000000);
    if (sem_wait(semaphoreReaders) != 0) errorHandler(9);
    workingReaders--;
    if (workingReaders == 0) {
      if (sem_post(semaphoreWriters) != 0) errorHandler(8);
    }
    if (sem_post(semaphoreReaders) != 0) errorHandler(8);
  //}
  return NULL;
}

void readData(int raport) {
  int numberOfNumbers = 0;
  for (int i = 0 ; i < DATA_SIZE ; i++) {
    if ((commonData[i] % READERS_DIVISOR) == 0) {
      numberOfNumbers++;
      if (raport == 1) printf("CZYTELNIK %ld znalazl w tablicy wartosc %d o numerze w tablicy %d.\n",pthread_self(),commonData[i],i);
    }
  }
  printf("CZYTELNIK %ld znalazl %d liczb podzielnych przez %d.\n",pthread_self(),numberOfNumbers,READERS_DIVISOR);
}

void * writersHandler(void * args) {
  ArgumentsForThread * arguments = (ArgumentsForThread*)args;
  //for (;;) {
    if (sem_wait(semaphoreWriters) != 0) errorHandler(9);
    writeData(arguments -> raport);
    //usleep(1000000);
    if (sem_post(semaphoreWriters) != 0) errorHandler(8);
  //}
  return NULL;
}

void writeData(int raport) {
  int numberOfNumbers = rand()%(WRITERS_HELP) + 1;
  int dataID,dataValue;
  for (int i = numberOfNumbers ; i >= 0 ; i--) {
    dataID = rand()%DATA_SIZE;
    dataValue = rand()%WRITERS_HELP;
    commonData[dataID] = dataValue;
    if (raport == 1) printf("PISARZ %ld zmienil w tablicy stara wartosc na wartosc %d o numerze w tablicy %d.\n",pthread_self(),dataValue,dataID);
  }
  printf("PISARZ %ld zmodyfikowal tablice.\n",pthread_self());
}

void waitForEndingThreads(ArgumentsForThread * arguments) {
  for (int i = 0 ; i < arguments -> numberOfReaders ; i++) {
    if (pthread_join(arguments -> threadsForReaders[i],NULL) !=0) errorHandler(6);
  }
  for (int i = 0 ; i < arguments -> numberOfWriters ; i++) {
    if (pthread_join(arguments -> threadsForWriters[i],NULL) !=0) errorHandler(6);
  }
}

void deleteArguments(ArgumentsForThread * arguments) {
  free(arguments -> threadsForReaders);
  free(arguments -> threadsForWriters);
  free(arguments);
  if (sem_destroy(semaphoreReaders) != 0) errorHandler(7);
  if (sem_destroy(semaphoreWriters) != 0) errorHandler(7);
}

void errorHandler(int number) {
  switch (number) {
    case 1: printf("ERROR! Niepoprawna ilosc argumentow! Powinny byc 2: ilosc watkow pisarzy, ilosc watkow czytelnikow i opcjonalnie \"-i\", gdy chcemy raport.\n"); exit(1);
    case 2: printf("ERROR! Niepoprawny argument! Ilosc musi byc liczba calkowita dodatnia.\n"); exit(2);
    case 3: printf("ERROR! Niepoprawny argument! Opcjonalna wartosc to \"-i\".\n"); exit(3);
    case 4: printf("ERROR! Niepoprawne wykonanie polecenia: sem_init!\n"); exit(4);
    case 5: printf("ERROR! Niepoprawne wykonanie polecenia: pthread_create!\n"); exit(5);
    case 6: printf("ERROR! Niepoprawne wykonanie polecenia: pthread_join!\n"); exit(6);
    case 7: printf("ERROR! Niepoprawne wykonanie polecenia: sem_destroy!\n"); exit(7);
    case 8: printf("ERROR! Niepoprawne wykonanie polecenia: sem_post!\n"); exit(8);
    case 9: printf("ERROR! Niepoprawne wykonanie polecenia: sem_wait!\n"); exit(9);
    default: printf("Brak takiego bledu!\n"); exit(0);
  }
}
