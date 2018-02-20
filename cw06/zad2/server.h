#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include "common.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

void createQueue();
void runningConnection();
void identifyMessage(struct Message message);
void serviceCloseClientQueue(struct Message message);
void serviceLogin(Message *message);
void addClient(pid_t clientPID, mqd_t clientQueue);
void sendResponse(mqd_t clientQueue, long typeOfMessage, char * textMessage);
void serviceEcho(struct Message message);
mqd_t getClientQueue(pid_t clientPID);
void serviceToCapital(struct Message message);
void serviceTime(struct Message message);
void serviceExitServer(struct Message message);
void closeClientsQueuesAndKillThem();
void closeQueue();
int queueEmpty();
void signalHandler(int signalType);
void errorHandler(int i);
