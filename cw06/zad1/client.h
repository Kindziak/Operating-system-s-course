#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include "common.h"

void getMainQueue();
void createQueue();
key_t createKey(int numberForGenerate);
void runningConnection();
struct Message clientWriteIn(struct Message message);
struct Message loggingToServer(struct Message message);
char * setCommunique();
enum TypeOfCommission parseLine();
enum TypeOfCommission stringToEnum(char * string);
void sendMessage(int clientQueue, long typeOfMessage, char * textMessage);
void identifyMessage(struct Message message);
void serviceLogin(struct Message message);
void serviceEcho(struct Message message);
void serviceToCapital(struct Message message);
void serviceTime(struct Message message);
void serviceExitServer(struct Message message);
void closeQueue();
void signalHandler(int signalType);
void errorHandler(int i);
