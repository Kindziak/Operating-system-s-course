#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/un.h>
#include <fcntl.h>
#include <poll.h>
#include "common.h"

void initializeArguments(int argc, char ** argv);
void prepareStructs();
void makeConnection();
void createThreads();
void * threadHandlerForPing(void * args);
void closeConnection(int number);
int chooseClient();
Commission * getData(char * buffer);
enum TypeOfCommission charToEnum(char type);
void * threadHandlerForData(void * arg);
void * threadHandlerForListener(void * arg);
int getAnswer(int x);
int findIndex();
void rejectClient(Commission message, int x);
int addClient(Commission message, int x);
void setRevents(int i);
void catchMessage(int number);
void exitOrPing(Commission message);
char enumTOChar(TypeOfCommission type);
void waitForEndingThreads();
void signalHandler(int signalType);
void errorHandler(int number);
