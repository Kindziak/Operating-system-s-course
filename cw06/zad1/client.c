#include "client.h"

int myQueue;
int mainQueue;
char clientQueueKey[10];

int main() {
  signal(SIGTSTP, signalHandler);
  createQueue();
  getMainQueue();
  runningConnection();
  return 0;
}

void getMainQueue() {
  if ((mainQueue = msgget(createKey(KEY_GENERATE_NUMBER), 0)) == -1) errorHandler(1);
}

void createQueue() {
  if ((myQueue = msgget(createKey(getpid()), IPC_CREAT | 0600)) == -1) errorHandler(1);
}

key_t createKey(int numberForGenerate) {
  key_t key;
  char * path;

  if(numberForGenerate == KEY_GENERATE_NUMBER) {
    if ((path = getenv(MAIN_QUEUE_PATH)) == NULL) errorHandler(2);
  }
  else path = ".";
  if ((key = ftok(path, numberForGenerate)) == -1) errorHandler(3);
  if (numberForGenerate != KEY_GENERATE_NUMBER) sprintf(clientQueueKey, "%d", key);
  return key;
}

void runningConnection() {
  printf("Klient zaczal dzialac!\n");

  struct Message message;

  message = loggingToServer(message);
  for (;;) {
    message = clientWriteIn(message);
    sendMessage(mainQueue, message.type, message.communique);
    if (msgrcv(myQueue, &message, MAX_SIZE_OF_MESSAGE, 0, 0) == -1) errorHandler(4);
    identifyMessage(message);
  }
}

struct Message clientWriteIn(struct Message message) {
  char line[MAX_LENGHT_OF_COMMUNIQUE];

  for(;;) {
    if ((message.type = parseLine()) == NIE) printf("Nie ma takiego typu. Podaj jeszcze raz.\n");
    else break;
  }
  if (message.type == ECHO || message.type == DUZA_LITERA) strcpy(message.communique, setCommunique(line));
  return message;
}

struct Message loggingToServer(struct Message message) {
  sendMessage(mainQueue, LOGOWANIE, clientQueueKey);
  if (msgrcv(myQueue, &message, MAX_SIZE_OF_MESSAGE, 0, 0) == -1) errorHandler(4);
  identifyMessage(message);
  return message;
}

char * setCommunique(char * line) {
  printf("Podaj komunikat do przeslania.\n");
  fgets(line, MAX_LENGHT_OF_COMMUNIQUE, stdin);
  return line;
}

enum TypeOfCommission parseLine() {
  char line[MAX_LENGHT_OF_COMMUNIQUE] = " ";

  while (strcmp(line," ") == 0 || strcmp(line,"\n") == 0) {
    printf("Podaj typ wiadomosci: ECHO, DUZA_LITERA, CZAS, NAKAZ_ZAMKNIECIA.\n");
    fgets(line, MAX_LENGHT_OF_COMMUNIQUE, stdin);
  }
  return stringToEnum(line);
}

enum TypeOfCommission stringToEnum(char * string) {
  if(string[strlen(string)-1] == '\n') string[strlen(string)-1] = '\0';
  for (int i = 0 ; i < sizeof(conversionTable) / sizeof(conversionTable[0]); i++) {
    if (!strcmp(string, conversionTable[i].typeAsString)) return conversionTable[i].typeAsEnum;
  }
  return NIE;
}

void sendMessage(int clientQueue, long typeOfMessage, char * textMessage) {
  printf("Wysylam wiadomosc.\n");

  struct Message response;
  response.type = typeOfMessage;
  response.senderPID = getpid();
  strcpy(response.communique, textMessage);

  if(msgsnd(clientQueue, &response, MAX_SIZE_OF_MESSAGE, 0) == -1) errorHandler(5);
}

void identifyMessage(struct Message message) {
  switch (message.type) {
    case AKCEPTACJA: serviceLogin(message); break;
    case ODMOWA: errorHandler(6); break;
    case ECHO: serviceEcho(message); break;
    case DUZA_LITERA: serviceToCapital(message); break;
    case CZAS: serviceTime(message); break;
    case NAKAZ_ZAMKNIECIA: serviceExitServer(message); break;
    default: printf("Nie ma takiego zlecenia!\n"); break;
  }
}

void serviceLogin(struct Message message) {
  printf("Zalogowany na serwerze.\n");
  printf("Moje ID to: %d", atoi(message.communique));
}

void serviceEcho(struct Message message) {
  printf("Otrzymalem od serwera: %s", message.communique);
}

void serviceToCapital(struct Message message) {
  printf("Otrzymalem od serwera: %s", message.communique);
}

void serviceTime(struct Message message) {
  printf("Otrzymalem od serwera: %s", message.communique);
}

void serviceExitServer(struct Message message) {
  closeQueue();
  exit(0);
}

void closeQueue() {
  msgctl(myQueue, IPC_RMID, NULL);
}

void signalHandler(int signalType) {
  switch (signalType) {
    case SIGTSTP:
      closeQueue();
      printf("Zakonczylem dzialanie przez zlecenie NAKAZ_ZAMKNIECIA.\n");
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
    case 6: printf("ERROR! Odmowa zalogowania!\n"); break;
    default: printf("Brak takiego bledu!\n"); break;
  }
  closeQueue();
  printf("Wylaczanie klienta z bledem.\n");
  exit(1);
}
