#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#include "FIFO.h"

void goToCut(int numberOfClients, int numberOfCuts);
void cutting(int numberOfClients, sigset_t mask, int numberOfCuts);
long getTime();
void createSemaphore();
void createWaitingRoomAsFIFO();
void attachFIFO();
void createKey();
void deleteSemaphoreAndFIFO();
void signalHandler(int signalType);
void errorHandler(int number);
