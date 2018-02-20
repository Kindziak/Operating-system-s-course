#define _GNU_SOURCE
#include "server.h"

nfds_t nfds=MAX_CLIENTS;

int portNumber;
char * socketPath;
struct pollfd pollEvents[MAX_CLIENTS+2];
Client clients[MAX_CLIENTS];
int pingedClients[MAX_CLIENTS];
int listenINET,listenUNIX;
struct pollfd pollINET,pollUNIX;
pthread_t threads[3];
int listenFd[2];
socklen_t socketLength;
socklen_t localAdressLength;
struct sockaddr_un sockAddrUn;

int main(int argc, char ** argv) {
  srand(time(NULL));
  signal(SIGINT,signalHandler);

  initializeArguments(argc,argv);
  prepareStructs();
  makeConnection();
  createThreads();
  waitForEndingThreads();
  return 0;
}

void initializeArguments(int argc, char ** argv) {
  if (argc != 3) errorHandler(1);
  portNumber = atoi(argv[1]);
  socketPath = malloc(strlen(argv[2]) +1);
  strcpy(socketPath,argv[2]);
}

void prepareStructs() {
  for (int i = 0 ; i < MAX_CLIENTS ; i++) {
    pollEvents[i].fd = -1;
    pollEvents[i].events = POLLRDHUP;
    strcpy(clients[i].nameOfClient,"");
    clients[i].descriptor = -1;
    pingedClients[i] = 0;
  }
  pollEvents[MAX_CLIENTS].events = POLLIN;
  pollEvents[MAX_CLIENTS +1].events = POLLIN;

}

void makeConnection() {
  struct sockaddr_in addressIn;
  struct sockaddr_un addressUn;

  memset(&addressIn,0,sizeof addressIn);
  memset(&addressUn,0,sizeof addressUn);
  addressIn.sin_family = AF_INET;
  addressIn.sin_addr.s_addr = htonl(INADDR_ANY);
  addressIn.sin_port = htons(portNumber);

  addressUn.sun_family = AF_UNIX;
  strcpy(addressUn.sun_path, socketPath);

  socketLength = sizeof addressIn;
  localAdressLength = sizeof addressUn;

  //pollINET.events = POLLIN;
  listenFd[0] = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
  //pollEvents[MAX_CLIENTS].fd = listenFd[0];
  //pollUNIX.events = POLLIN;
  listenFd[1] = socket(AF_UNIX,SOCK_DGRAM,0);
  //pollEvents[MAX_CLIENTS+1].fd = listenFd[1];

  bind(listenFd[0],(struct sockaddr*)&addressIn,sizeof(struct sockaddr_in));
  bind(listenFd[1],(struct sockaddr*)&addressUn,sizeof(struct sockaddr_un));
}

void createThreads() {
  if (pthread_create(&threads[0],NULL,threadHandlerForListener,NULL) != 0) errorHandler(5);
  if (pthread_create(&threads[1],NULL,threadHandlerForData,NULL) != 0) errorHandler(5);
  if (pthread_create(&threads[2],NULL,threadHandlerForPing,NULL) != 0) errorHandler(5);
}

void * threadHandlerForPing(void * args) {
  //Commission message;
  //message.operation = PING;
  while (1) {
    sleep(5);
    for (int i = 0 ; i < MAX_CLIENTS ; i++) {
      /*if (clients[i].descriptor <= 0) continue;
      else {
        if (pingedClients[i] == 1) {
          closeConnection(i);
          continue;
        }
        write(clients[i].descriptor,(void*)&message,sizeof(Commission));
        pingedClients[i] = 1;
      }*/
    }
  }
  return (void *)NULL;
}

void closeConnection(int number) {
  printf("Klient %s nie odpowiada. Polaczenie zostaje zerwane.\n",clients[number].nameOfClient);
  shutdown(clients[number].descriptor,SHUT_RDWR);
  close(clients[number].descriptor);
  pollEvents[number].fd = -1;
  pingedClients[number] = 0;
  clients[number].descriptor = -1;
  strcpy(clients[number].nameOfClient,"");
}

int chooseClient() {
  int workingClients[MAX_CLIENTS];
  int maxWorkingCLients = 0;
  for (int i = 0 ; i < MAX_CLIENTS ; i++) {
    if (clients[i].descriptor > 0) {
      workingClients[maxWorkingCLients] = i;
      maxWorkingCLients++;
    }
  }
  if (maxWorkingCLients != 0) return workingClients[rand() % maxWorkingCLients];
  else return -1;
}

Commission * getData(char * buffer) {
  char firstNumber[10], secondNumber[10];
  char operator;
  int a,b;
  for (a = 0 ; a < strlen(buffer) && buffer[a] >= '0' && buffer[a] <= '9' ; a++) firstNumber[a] = buffer[a];
  operator = buffer[a];
  firstNumber[a] = '\0';
  a++;
  for (b = 0 ; a < strlen(buffer) ; b++) {
    secondNumber[b] = buffer[a];
    a++;
  }
  secondNumber[b] = '\0';

  Commission * message = malloc(sizeof(Commission));
  message -> argument1 = atoi(firstNumber);
  message -> argument2 = atoi(secondNumber);
  message -> operation = charToEnum(operator);
  return message;
}

enum TypeOfCommission charToEnum(char type) {
  for (int i = 0 ; i < sizeof(conversionTableForOperators) / sizeof(conversionTableForOperators[0]); i++) {
    if (conversionTableForOperators[i].typeAsChar == type) return conversionTableForOperators[i].typeAsEnum;
  }
  return ODMOWA;
}

void * threadHandlerForData(void * arg) {
  char buffer[1024];
  Commission * message;
  int clientID;
  while(1) {
    if ((clientID = chooseClient()) >= 0) {
      printf("Wpisz dzialanie.\n");
      fgets(buffer,1000,stdin);
      message = getData(buffer);
      sendto(listenFd[1],(void*)&message,sizeof(Commission),0,(struct sockaddr *)&clients[clientID].sockAddrUnStruct,localAdressLength );
      //write(clients[clientID].descriptor,(void*)message,sizeof(Commission));
      free(message);
    }
    //else printf("Nie ma klientow.\n");
  }
  return (void*)NULL;
}

void * threadHandlerForListener(void*arg) {
  struct timeval helpTime;
  helpTime.tv_sec = 0;
  helpTime.tv_usec = 200001;

  setsockopt(listenFd[1], SOL_SOCKET, SO_RCVTIMEO, &helpTime, sizeof helpTime);
  while(1) {
    char buffer[70] = {'\0'};
    if (recvfrom(listenFd[1],buffer,sizeof buffer, 0, (struct sockaddr *)&sockAddrUn,&localAdressLength) > 0) {
      printf("%s\n",buffer);
      orderHandler(buffer,1);
    }
  }

}

void orderHandler(char * buffer, int typ) {
  Commission message = *(Commission*)buffer;
  Client c;
  int result;
  if (message.operation == NAZWA) {
    strcpy(c.nameOfClient,message.nameOfClient);
    c.sockAddrUnStruct = sockAddrUn;
    c.descriptor = 1;
    printf("%s\n",message.nameOfClient);
    result = addClient(message,c);
    if (result != -1) {
      clients[result] = c;
      printf("wwiiii\n");
    }
  }
  else {
    //char buffer[1024];
    //recvfrom(listenFd[1],buffer,sizeof buffer, 0, (struct sockaddr *)&sockAddrUn,&localAdressLength);
    printf("%s,\n",buffer);
    //read(pollEvents[number].fd,buffer,sizeof(Commission));
    //Commission message = *(Commission*)buffer;
    switch (message.operation) {
      case DODAJ: case ODEJMIJ: case POMNOZ: case PODZIEL:
        printf("Klient %s obliczyl wyrazenie %d %c %d = %d\n",message.nameOfClient,message.argument1,enumTOChar(message.operation),message.argument2,message.result);
        break;
      case WYJSCIE: case PING:
        exitOrPing(message);
        break;
      default: break;
    }
  }
}

int addClient(Commission message, Client c) {
  int index;
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if(clients[i].descriptor == 1) index = -1;
    else index = i;
  }
  printf("zasj\n");
  sendto(listenFd[1],(void *)&message,sizeof message,0,(struct sockaddr *)&c.sockAddrUnStruct,localAdressLength);
  return index;
}

void * threadHandlerForListener2(void * arg) {
  listen(listenFd[0],MAX_CLIENTS);
  listen(listenFd[1],MAX_CLIENTS);

  int x;
  for (;;) {
    //printf("lol\n");
    for (int i = 0 ; i < MAX_CLIENTS ; i++) pollEvents[i].events = POLLIN;
    //printf("lol\n");
    poll(pollEvents, MAX_CLIENTS+2, -1);
    //printf("lol\n");
    for (int i = 0 ; i < MAX_CLIENTS ; i++) {
      if ((pollEvents[i].revents & (POLLRDHUP | POLLHUP)) != 0) setRevents(i);
      else if ((pollEvents[i].revents & POLLIN) != 0) {
        //printf("lol\n");
        catchMessage(i);
      }
    }

    //printf("lol\n");
    while ((x = accept(pollEvents[MAX_CLIENTS].fd,NULL,NULL)) > 0) {
      //printf("lol\n");
      if (getAnswer(x) == -1) break;
    }
    //printf("lol\n");
    while ((x = accept(pollEvents[MAX_CLIENTS+1].fd,NULL,NULL)) > 0) {
      //printf("lol\n");
      if (getAnswer(x) == -1) break;
    }
  }
  return (void*)NULL;
}

int getAnswer(int x) {
  char buffer[1024];
  read(x,buffer,sizeof(Commission));
  Commission message = *(Commission*)buffer;
  int isClient = 0;
  for (int i = 0 ; i < MAX_CLIENTS ; i++) {
    if (strcmp(clients[i].nameOfClient,message.nameOfClient) == 0 && clients[i].descriptor > 0) {
      isClient = -1;
      break;
    }
  }
  if (isClient == -1) {
    rejectClient(message,x);
    return -1;
  }
  return 0;
  //else return addClient(message,x);
}

int findIndex() {
  for (int i = 0 ; i < MAX_CLIENTS ; i++) {
    if (clients[i].descriptor <= 0) return i;
  }
  return -1;
}

void rejectClient(Commission message, int x) {
  message.operation = ODMOWA;
  write(x,(void*)&message,sizeof(Commission));
  shutdown(x,SHUT_RDWR);
  close(x);
}

int addClient1(Commission message, int x) {
  int indexForNewClient = findIndex();
  if (indexForNewClient == -1) {
    rejectClient(message,x);
    return -1;
  }
  else {
    message.operation = AKCEPTACJA;
    write(x,(void*)&message,sizeof(Commission));
    pollEvents[indexForNewClient].fd = x;
    strcpy(clients[indexForNewClient].nameOfClient,message.nameOfClient);
    clients[indexForNewClient].descriptor = x;
  }
  return 0;
}

void setRevents(int i) {
  clients[i].descriptor = -1;
  strcpy(clients[i].nameOfClient, "");
  pollEvents[i].fd = -1;
  pingedClients[i] = 0;
}

void catchMessage(int number) {
  char buffer[1024];

  //read(pollEvents[number].fd,buffer,sizeof(Commission));
  Commission message = *(Commission*)buffer;
  switch (message.operation) {
    case DODAJ: case ODEJMIJ: case POMNOZ: case PODZIEL:
      printf("Klient %s obliczyl wyrazenie %d %c %d = %d\n",message.nameOfClient,message.argument1,enumTOChar(message.operation),message.argument2,message.result);
      break;
    case WYJSCIE: case PING:
      exitOrPing(message);
      break;
    default: break;
  }
}

void exitOrPing(Commission message) {
  int i = 0;
  while (i < MAX_CLIENTS) {
    if (strcmp(clients[i].nameOfClient,message.nameOfClient) == 0) break;
    i++;
  }
  if (message.operation == PING) pingedClients[i] = 0;
  else shutdown(pollEvents[i].fd,SHUT_RDWR);
}

char enumTOChar(TypeOfCommission type) {
  for (int i = 0 ; i < sizeof(conversionTableForOperators) / sizeof(conversionTableForOperators[0]); i++) {
    if (conversionTableForOperators[i].typeAsEnum == type) return conversionTableForOperators[i].typeAsChar;
  }
  return '=';
}

void waitForEndingThreads() {
  for (int i = 0 ; i < 3 ; i++) {
    if (pthread_join(threads[i],NULL) !=0) errorHandler(6);
  }
}

void signalHandler(int signalType) {
  switch (signalType) {
    case SIGINT:
      unlink(socketPath);
      exit(0);
      break;
    default: break;
  }
}

void errorHandler(int number) {
  switch (number) {
    case 1: printf("ERROR! Niepoprawna ilosc argumentow! Powinny byc 2: numer portu TCP/UDP, sciezke gniazda UNIX.\n"); exit(1);
    //case 2: printf("ERROR! Niepoprawny argument! Ilosc watkow musi byc liczba calkowita dodatnia.\n"); exit(2);
    //case 3: printf("ERROR! Niepoprawny argument! Ilosc rekordow czytanych przez watek w pojedynczym odczycie musi byc liczba calkowita dodatnia.\n"); exit(3);
    //case 4: printf("ERROR! Niepoprawne wykonanie polecenia: open!\n"); exit(4);
    case 5: printf("ERROR! Niepoprawne wykonanie polecenia: pthread_create!\n"); exit(5);
    case 6: printf("ERROR! Niepoprawne wykonanie polecenia: pthread_join!\n"); exit(6);
    //case 7: printf("ERROR! Niepoprawne wykonanie polecenia: pthread_setcanceltype!\n"); exit(7);
    //case 8: printf("ERROR! Niepoprawne wykonanie polecenia: read!\n"); exit(8);
    //case 9: printf("ERROR! Niepoprawne wykonanie polecenia: close!\n"); exit(9);
    default: printf("Brak takiego bledu!\n"); exit(0);
  }
}
