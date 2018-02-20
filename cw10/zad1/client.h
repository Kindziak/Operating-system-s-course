#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netdb.h>
#include "common.h"

#define SIEC 0
#define LOKALNIE 1

void work() ;
void doCalculations(Commission * message);
char enumTOChar(TypeOfCommission type);
void connectNetwork();
void connectLocal();
enum TypeOfCommission sendName();
void makeConnection();
void initializeArguments(int argc, char ** argv);
void signalHandler(int signalType);
void errorHandler(int number);
