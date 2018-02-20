#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include "FIFO.h"

#define FIFO_PATH "HOME"
#define KEY_GENERATE_NUMBER 5
#define AMOUNT_OF_SEMAPHORES 4

void goToCut(int numberOfClients, int numberOfCuts,int isEnd);
void cutting(int numberOfClients, sigset_t mask, int numberOfCuts);
long getTime();
void createSemaphore();
void createWaitingRoomAsFIFO();
void attachFIFO();
void createKey();
void deleteSemaphoreAndFIFO();
void signalHandler(int signalType);
void errorHandler(int number);
