#include "server.h"

int mainQueue;
int numberOfClients = 0;
int clients[MAX_CLIENTS][2];

int main() {
  signal(SIGTSTP, signalHandler);
  createQueue();
  runningConnection();
  return 0;
}

void createQueue() {
  if ((mainQueue = msgget(createKey(), IPC_CREAT | 0600)) == -1) errorHandler(1);
}

key_t createKey() {
  key_t key;
  char * path;
  if ((path = getenv(MAIN_QUEUE_PATH)) == NULL) errorHandler(2);
  if ((key = ftok(path, KEY_GENERATE_NUMBER)) == -1) errorHandler(3);
  return key;
}

void runningConnection() {
  printf("Serwer zaczal dzialac!\n");
  struct Message message;
  for (;;) {
    if (msgrcv(mainQueue, &message, MAX_SIZE_OF_MESSAGE, 0, 0) == -1) errorHandler(4);
    identifyMessage(message);
  }
}

void identifyMessage(struct Message message) {
  switch (message.type) {
    case LOGOWANIE: serviceLogin(message); break;
    case ECHO: serviceEcho(message); break;
    case DUZA_LITERA: serviceToCapital(message); break;
    case CZAS: serviceTime(message); break;
    case NAKAZ_ZAMKNIECIA: serviceExitServer(message); break;
    default: printf("Nie ma takiego zlecenia!\n"); break;
  }
}

void serviceLogin(struct Message message) {
  printf("Loguje klienta.\n");

  int clientQueue;
  char numberOfClientsAsString[MAX_LENGHT_OF_COMMUNIQUE];
  pid_t clientPID = message.senderPID;

  if ((clientQueue = msgget(atoi(message.communique), 0)) == -1) errorHandler(1);

  if (numberOfClients == MAX_CLIENTS) sendResponse(clientQueue, ODMOWA, "");
  else {
    sprintf(numberOfClientsAsString, "%d", numberOfClients);
    sendResponse(clientQueue, AKCEPTACJA, numberOfClientsAsString);
    addClient(clientPID, clientQueue);
  }
}

void addClient(pid_t clientPID, int clientQueue) {
  printf("Dodaje kilenta.\n");

  clients[numberOfClients][0] = clientPID;
  clients[numberOfClients][1] = clientQueue;
  numberOfClients ++;
}

void sendResponse(int clientQueue, long typeOfMessage, char * textMessage) {
  printf("Wysylam odpowiedz.\n");

  struct Message response;
  response.type = typeOfMessage;
  response.senderPID = getpid();
  strcpy(response.communique, textMessage);

  if(msgsnd(clientQueue, &response, MAX_SIZE_OF_MESSAGE, 0) == -1) errorHandler(5);
}

void serviceEcho(struct Message message) {
  printf("Dostalem zlecenie ECHO.\n");

  int clientQueue;

  if((clientQueue = getClientQueue(message.senderPID)) == -1){
    printf("ERROR! Nie ma takiego klienta w obsludze serwera!\n");
    return;
  }

  sendResponse(clientQueue, ECHO, message.communique);
}

int getClientQueue(pid_t clientPID) {
  for (int i = 0 ; i < numberOfClients ; i++) if (clientPID == clients[i][0]) return clients[i][1];
  return -1;
}

void serviceToCapital(struct Message message) {
  printf("Dostalem zlecenie DUZA_LITERA.\n");

  int clientQueue;
  char * toCapital = message.communique;

  if((clientQueue = getClientQueue(message.senderPID)) == -1) {
    printf("ERROR! Nie ma takiego klienta w obsludze serwera!\n");
    return;
  }
  for(int i = 0 ; toCapital[i] ; i++) toCapital[i] = (char)(toupper(toCapital[i]));
  sendResponse(clientQueue, 3, toCapital);
}

void serviceTime(struct Message message) {
  printf("Dostalem zlecenie CZAS.\n");

  int clientQueue;
  time_t currentTime;

  if((clientQueue = getClientQueue(message.senderPID)) == -1) {
    printf("ERROR! Nie ma takiego klienta w obsludze serwera!\n");
    return;
  }
  time(&currentTime);
  sendResponse(clientQueue, 3, asctime(localtime(&currentTime)));
}

void serviceExitServer(struct Message message) {
  printf("Dostalem zlecenie NAKAZ_ZAMKNIECIA.\n");
  printf("Serwer konczy swoje dzialanie.\n");

  for (;;) {
    if (msgrcv(mainQueue, &message, MAX_SIZE_OF_MESSAGE, 0, IPC_NOWAIT) == -1) break;
    identifyMessage(message);
  }
  killClients();
  closeQueue();
  
  printf("Koniec.\n");
  exit(0);
}

void killClients() {
  for (int i = 0 ; i < numberOfClients ; i++) kill(clients[i][0], SIGTSTP);
}

void closeQueue() {
  msgctl(mainQueue, IPC_RMID, NULL);
}

void signalHandler(int signalType) {
  switch (signalType) {
    case SIGTSTP:
      closeQueue();
      killClients();
      exit(0);
      break;
    default: break;
  }
}

void errorHandler(int i) {
  switch (i) {
    case 1: printf("ERROR! Niepoprawne wykonanie polecenia msgget!\n"); break;
    case 2: printf("ERROR! Niepoprawne wykonanie polecenia getenv!\n"); break;
    case 3: printf("ERROR! Niepoprawne wykonanie polecenia ftok!\n"); break;
    case 4: printf("ERROR! Niepoprawne wykonanie polecenia msgrcv!\n"); break;
    case 5: printf("ERROR! Niepoprawne wykonanie polecenia msgsnd!\n"); break;
    default: printf("Brak takiego bledu!\n"); break;
  }
  closeQueue();
  killClients();
  printf("Wylaczanie serwera z bledem.\n");
  exit(1);
}
