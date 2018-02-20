#include "server.h"

mqd_t mainQueue;
int numberOfClients = 0;
int clients[MAX_CLIENTS][2];

int main() {
  signal(SIGTSTP, signalHandler);
  createQueue();
  runningConnection();
  return 0;
}

void createQueue() {
  struct mq_attr attr;
  attr.mq_maxmsg = MAX_CLIENTS;
  attr.mq_msgsize = MAX_SIZE_OF_MESSAGE;

  if ((mainQueue = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0600, &attr)) == -1) errorHandler(1);
}

void runningConnection() {
  printf("Serwer zaczal dzialac!\n");

  struct Message message;

  for (;;) {
    if (mq_receive(mainQueue, (char *)&message, MAX_LENGHT_OF_COMMUNIQUE, 0) == -1) errorHandler(2);
    identifyMessage(message);
  }
}

void identifyMessage(struct Message message) {
  switch (message.type) {
    case LOGOWANIE: serviceLogin(&message); break;
    case ECHO: serviceEcho(message); break;
    case DUZA_LITERA: serviceToCapital(message); break;
    case CZAS: serviceTime(message); break;
    case NAKAZ_ZAMKNIECIA: serviceExitServer(message); break;
    case ZAMYKANIE_KOLEJKI: serviceCloseClientQueue(message); break;
    default: printf("Nie ma takiego zlecenia!\n"); break;
  }
}

void serviceCloseClientQueue(struct Message message) {
  for (int i = 0 ; i < numberOfClients ; i++) {
    if (clients[i][0] == message.senderPID) {
      mq_close(clients[i][1]);
      break;
    }
  }
}

void serviceLogin(Message *message) {
  printf("Loguje klienta.\n");

  mqd_t clientQueue;
  char numberOfClientsAsString[MAX_LENGHT_OF_COMMUNIQUE];
  pid_t clientPID = message->senderPID;
  char * clientName = message->communique;

  if ((clientQueue = mq_open(clientName, O_WRONLY)) == -1) errorHandler(1);

  if (numberOfClients == MAX_CLIENTS) sendResponse(clientQueue, ODMOWA, "");
  else {
    snprintf(numberOfClientsAsString, MAX_LENGHT_OF_COMMUNIQUE, "%d", numberOfClients);
    sendResponse(clientQueue, AKCEPTACJA, numberOfClientsAsString);
    addClient(clientPID, clientQueue);
  }
}

void addClient(pid_t clientPID, mqd_t clientQueue) {
  printf("Dodaje kilenta.\n");

  clients[numberOfClients][0] = clientPID;
  clients[numberOfClients][1] = clientQueue;
  numberOfClients ++;
}

void sendResponse(mqd_t clientQueue, long typeOfMessage, char * textMessage) {
  printf("Wysylam odpowiedz.\n");

  struct Message response;
  response.type = typeOfMessage;
  response.senderPID = getpid();
  strcpy(response.communique, textMessage);

  if (mq_send(clientQueue, (char *)&response, MAX_LENGHT_OF_COMMUNIQUE, 0) == -1) errorHandler(3);
}

void serviceEcho(struct Message message) {
  printf("Dostalem zlecenie ECHO.\n");

  mqd_t clientQueue;

  if((clientQueue = getClientQueue(message.senderPID)) == -1){
    printf("ERROR! Nie ma takiego klienta w obsludze serwera!\n");
    return;
  }

  sendResponse(clientQueue, ECHO, message.communique);
}

mqd_t getClientQueue(pid_t clientPID) {
  for (int i = 0 ; i < numberOfClients ; i++) if (clientPID == clients[i][0]) return clients[i][1];
  return -1;
}

void serviceToCapital(struct Message message) {
  printf("Dostalem zlecenie DUZA_LITERA.\n");

  mqd_t clientQueue;
  char * toCapital = message.communique;

  if((clientQueue = getClientQueue(message.senderPID)) == -1) {
    printf("ERROR! Nie ma takiego klienta w obsludze serwera!\n");
    return;
  }
  for(int i = 0 ; toCapital[i] ; i++) toCapital[i] = (char)(toupper(toCapital[i]));
  sendResponse(clientQueue, DUZA_LITERA, toCapital);
}

void serviceTime(struct Message message) {
  printf("Dostalem zlecenie CZAS.\n");

  mqd_t clientQueue;
  time_t currentTime;

  if((clientQueue = getClientQueue(message.senderPID)) == -1) {
    printf("ERROR! Nie ma takiego klienta w obsludze serwera!\n");
    return;
  }
  time(&currentTime);
  sendResponse(clientQueue, CZAS, asctime(localtime(&currentTime)));
}

void serviceExitServer(struct Message message) {
  printf("Dostalem zlecenie NAKAZ_ZAMKNIECIA.\n");
  printf("Serwer konczy swoje dzialanie.\n");

  while (!queueEmpty()) {
    if (mq_receive(mainQueue, (char *)&message, MAX_LENGHT_OF_COMMUNIQUE, 0) == -1) errorHandler(2);
    identifyMessage(message);
  }
  closeClientsQueuesAndKillThem();
  closeQueue();

  printf("Koniec.\n");
  exit(0);
}

int queueEmpty() {
  struct mq_attr attr;
  if (mq_getattr(mainQueue, &attr)) errorHandler(4);
  return (attr.mq_curmsgs == 0);
}

void closeClientsQueuesAndKillThem() {
  for (int i = 0 ; i < numberOfClients ; i++) {
    mq_close(clients[i][1]);
    kill(clients[i][0], SIGUSR1);
  }
}

void closeQueue() {
  mq_close(mainQueue);
  mq_unlink(QUEUE_NAME);
}

void signalHandler(int signalType) {
  switch (signalType) {
    case SIGTSTP:
      closeClientsQueuesAndKillThem();
      closeQueue();
      exit(0);
      break;
    default: break;
  }
}

void errorHandler(int i) {
  switch (i) {
    case 1: printf("ERROR! Niepoprawne wykonanie polecenia mq_open!\n"); break;
    case 2: printf("ERROR! Niepoprawne wykonanie polecenia mq_receive!\n"); break;
    case 3: printf("ERROR! Niepoprawne wykonanie polecenia mq_send!\n"); break;
    case 4: printf("ERROR! Niepoprawne wykonanie polecenia mq_getattr!\n"); break;
    default: printf("Brak takiego bledu!\n"); break;
  }
  closeClientsQueuesAndKillThem();
  closeQueue();
  printf("Wylaczanie serwera z bledem.\n");
  exit(1);
}
