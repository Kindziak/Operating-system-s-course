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
#include <limits.h>
#include <time.h>
#include <errno.h>

void getMainQueue();
void createQueue();
void giveQueueName();
void runningConnection();
struct Message clientWriteIn(struct Message message);
Message loggingToServer(Message message);
char * setCommunique();
enum TypeOfCommission parseLine();
enum TypeOfCommission stringToEnum(char * string);
void identifyMessage(struct Message message);
void serviceLogin(struct Message message);
Message sendMessage(int clientQueue, long typeOfMessage, char * textMessage);
void serviceEcho(struct Message message);
void serviceToCapital(struct Message message);
void serviceTime(struct Message message);
void serviceExitServer(struct Message message);
void closeQueue();
void signalHandler(int signalType);
void errorHandler(int i);
