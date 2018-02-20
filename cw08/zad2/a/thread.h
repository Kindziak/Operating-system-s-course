#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#define  RECORD_SIZE 1024

typedef struct ArgumentsForThread {
  pthread_t * threads;
  int numberOfThreads;
  int file;
  int numberOfRecordsForThread;
  char * word;
  int waitingForStartingThreads;
  int numberOfTest;
  int numberOfSignal;
} ArgumentsForThread;

ArgumentsForThread * initializeArguments(int argc, char ** argv, ArgumentsForThread * arguments);
int prepareFile(char * fileName);
ArgumentsForThread * createThreads(ArgumentsForThread * arguments);
void * threadHandler(void * args);
void setTestSignals(ArgumentsForThread * arguments);
char ** readFile(ArgumentsForThread * arguments, char ** readRecords);
int lookingForWord(ArgumentsForThread * arguments, char ** readRecords);
int getRecordID(char ** readRecords, int i);
void cancelRestThreads(ArgumentsForThread * arguments);
void switchTest(ArgumentsForThread * arguments);
void signalHandler(int numberOfSignal);
void waitForEndingThreads(ArgumentsForThread * arguments);
void deleteArguments(ArgumentsForThread * arguments);
void errorHandler(int number);
