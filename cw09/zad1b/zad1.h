#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/syscall.h>

#define READERS_DIVISOR 3
#define DATA_SIZE 10
#define WRITERS_HELP 3

typedef struct ArgumentsForThread {
  int threadID;
  pthread_t * threadsForReaders;
  pthread_t * threadsForWriters;
  int numberOfReaders;
  int numberOfWriters;
  int raport;
} ArgumentsForThread;

ArgumentsForThread * initializeArguments(int argc, char ** argv, ArgumentsForThread * arguments);
void createFIFO();
void prepareData();
ArgumentsForThread * createThreads(ArgumentsForThread * arguments);
void * readersHandler(void * args);
void * writersHandler(void * args);
void readData(int raport);
void writeData(int raport);
void waitForEndingThreads(ArgumentsForThread * arguments);
void deleteArguments(ArgumentsForThread * arguments);
void errorHandler(int number);
