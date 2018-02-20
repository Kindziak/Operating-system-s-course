#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

#ifndef FIFO_H
#define FIFO_H

typedef struct FIFOqueue {
  int firstClient;
  int lastClient;
  int maxNumberOfWaitingClients;
  int currentNumber;
  pid_t clientToCut;
  pid_t waitingRoom[100];
} FIFOqueue;

void initializeFIFO(FIFOqueue * fifo, int maxSize);
int isFullFIFO(FIFOqueue * fifo);
int isEmptyFIFO(FIFOqueue * fifo);
int pushFIFO(FIFOqueue * fifo, pid_t client);
pid_t popFIFO(FIFOqueue * fifo);

#endif
