#include "FIFO.h"

void initializeFIFO(FIFOqueue * fifo, int maxSize) {
  fifo -> firstClient = 0;
  fifo -> lastClient = 0;
  fifo -> maxNumberOfWaitingClients = maxSize;
  fifo -> currentNumber = 0;
  fifo -> clientToCut = 0;
}

int isFullFIFO(FIFOqueue * fifo) {
  if (fifo -> currentNumber <=fifo -> maxNumberOfWaitingClients) return 1;
  else return 0;
}

int isEmptyFIFO(FIFOqueue * fifo) {
  if (fifo -> currentNumber != 0) return 1;
  else return 0;
}

int pushFIFO(FIFOqueue * fifo, pid_t client) {
  if (isFullFIFO(fifo) == 1) {
    fifo -> currentNumber++;
    fifo -> waitingRoom[fifo -> lastClient] = client;
    if (fifo -> lastClient +1 >= 100) fifo -> lastClient = 0;
    else fifo -> lastClient++;
    return 0;
  }
  return -1;
}

pid_t popFIFO(FIFOqueue * fifo) {
  if (isEmptyFIFO(fifo) == 1) {
    fifo -> currentNumber--;
    pid_t client = fifo -> waitingRoom[fifo -> firstClient];
    if (fifo -> firstClient  + 1 >= 100) fifo -> firstClient = 0;
    else fifo -> firstClient++;
    return client;
  }
  return -1;
}
