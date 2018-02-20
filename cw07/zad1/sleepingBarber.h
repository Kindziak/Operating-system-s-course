#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include "FIFO.h"

#define FIFO_PATH "HOME"

void workAndSleep();
long getTime();
void cut(pid_t clientToCut);
void createSemaphore();
void createWaitingRoomAsFIFO(int numberOfChairs);
void attachFIFO();
void createKey();
void deleteSemaphoreAndFIFO();
void signalHandler(int signalType);
void errorHandler(int number);
