#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include "common.h"

void createQueue();
key_t createKey();
void runningConnection();
int getClientQueue(pid_t clientPID);
void identifyMessage(struct Message message);
void serviceLogin(struct Message message);
void addClient(pid_t clientPID, int clientQueue);
void sendResponse(int clientQueue, long typeOfMessage, char * textMessage);
void serviceEcho(struct Message message);
void serviceToCapital(struct Message message);
void serviceTime(struct Message message);
void serviceExitServer(struct Message message);
void killClients();
void closeQueue();
void signalHandler(int signalType);
void errorHandler(int i);
