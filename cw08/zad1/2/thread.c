#include "thread.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char ** argv) {
  ArgumentsForThread * arguments = calloc(1,sizeof(ArgumentsForThread));

  arguments = initializeArguments(argc,argv,arguments);
  arguments = createThreads(arguments);
  waitForEndingThreads(arguments);
  deleteArguments(arguments);

  return 0;
}

ArgumentsForThread * initializeArguments(int argc, char ** argv, ArgumentsForThread * arguments) {
  if (argc != 5) errorHandler(1);
  if ((arguments -> numberOfThreads = atoi(argv[1])) <= 0) errorHandler(2);
  if ((arguments -> numberOfRecordsForThread = atoi(argv[3])) <= 0) errorHandler(3);
  arguments -> word = argv[4];
  arguments -> file = prepareFile(argv[2]);
  arguments -> threads = calloc(arguments -> numberOfThreads,sizeof(pthread_t));
  arguments -> waitingForStartingThreads = 1;
  return arguments;
}

int prepareFile(char * fileName) {
  int file;
  if ((file = open(fileName,O_RDONLY)) == -1) errorHandler(4);
  return file;
}

ArgumentsForThread * createThreads(ArgumentsForThread * arguments) {
  for (int i = 0 ; i < arguments -> numberOfThreads ; i++) {
    if (pthread_create(&(arguments -> threads[i]),NULL,threadHandler,(void *)arguments) != 0) errorHandler(5);
  }
  return arguments;
}

void * threadHandler(void * args) {
  ArgumentsForThread * arguments = (ArgumentsForThread*)args;
  char ** readRecords = calloc(arguments -> numberOfRecordsForThread,sizeof(char*));
  for (int i = 0 ; i < arguments -> numberOfRecordsForThread ; i++) readRecords[i] = calloc(RECORD_SIZE,sizeof(char));

  if (pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL) != 0) errorHandler(7);
  while (arguments -> waitingForStartingThreads);
  for (int stillWorking = 1 ; stillWorking ;) {
    readRecords = readFile(arguments,readRecords);
    stillWorking = lookingForWord(arguments,readRecords);
  }

  for (int i = 0 ; i < arguments -> numberOfRecordsForThread ; i++) free(readRecords[i]);
  free(readRecords);
  return NULL;
}

char ** readFile(ArgumentsForThread * arguments, char ** readRecords) {
  pthread_mutex_lock(&mutex);
  for (int i =0 ; i < arguments -> numberOfRecordsForThread ; i++) {
    if (read(arguments -> file,readRecords[i],RECORD_SIZE) == -1) errorHandler(8);
  }
  pthread_mutex_unlock(&mutex);
  usleep(1);
  pthread_testcancel();
  return readRecords;
}

int lookingForWord(ArgumentsForThread * arguments, char ** readRecords) {
  for (int i = 0 ; i < arguments -> numberOfRecordsForThread ; i++) {
    if (strstr(readRecords[i],arguments -> word) != NULL) {
      printf("Watek o identyfikatorze: %ld, znalazl szukane slowo: %s w rekordzie o id: %d.\n",pthread_self(),arguments -> word,getRecordID(readRecords,i));
      cancelRestThreads(arguments);
      return 0;
    }
  }
  return 1;
}

int getRecordID(char ** readRecords, int i) {
  char * parsedRecord = strtok(readRecords[i]," ");
  return atoi(parsedRecord);
}

void cancelRestThreads(ArgumentsForThread * arguments) {
  for (int i = 0 ; i < arguments -> numberOfThreads ; i++) {
    if (arguments -> threads[i] != pthread_self()) pthread_cancel(arguments -> threads[i]);
  }
}

void waitForEndingThreads(ArgumentsForThread * arguments) {
  arguments -> waitingForStartingThreads = 0;
  for (int i = 0 ; i < arguments -> numberOfThreads ; i++) {
    if (pthread_join(arguments -> threads[i],NULL) !=0) errorHandler(6);
  }
}

void deleteArguments(ArgumentsForThread * arguments) {
  free(arguments -> threads);
  free(arguments);
  if (close(arguments -> file) == -1) errorHandler(9);
}

void errorHandler(int number) {
  switch (number) {
    case 1: printf("ERROR! Niepoprawna ilosc argumentow! Powinny byc 4: ilosc watkow, nazwa pliku, ilosc rekordow czytanych przez watek w pojedynczym odczycie i slowo do wyszukania w rekordzie.\n"); exit(1);
    case 2: printf("ERROR! Niepoprawny argument! Ilosc watkow musi byc liczba calkowita dodatnia.\n"); exit(2);
    case 3: printf("ERROR! Niepoprawny argument! Ilosc rekordow czytanych przez watek w pojedynczym odczycie musi byc liczba calkowita dodatnia.\n"); exit(3);
    case 4: printf("ERROR! Niepoprawne wykonanie polecenia: open!\n"); exit(4);
    case 5: printf("ERROR! Niepoprawne wykonanie polecenia: pthread_create!\n"); exit(5);
    case 6: printf("ERROR! Niepoprawne wykonanie polecenia: pthread_join!\n"); exit(6);
    case 7: printf("ERROR! Niepoprawne wykonanie polecenia: pthread_setcanceltype!\n"); exit(7);
    case 8: printf("ERROR! Niepoprawne wykonanie polecenia: read!\n"); exit(8);
    case 9: printf("ERROR! Niepoprawne wykonanie polecenia: close!\n"); exit(9);
    default: printf("Brak takiego bledu!\n"); exit(0);
  }
}
