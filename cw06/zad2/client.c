#include "client.h"

mqd_t myQueue;
mqd_t mainQueue;
char clientQueueName[14];

int main() {
  srand(time(NULL));
  signal(SIGTSTP, signalHandler);
  signal(SIGUSR1, signalHandler);
  createQueue();
  getMainQueue();
  runningConnection();
  return 0;
}

void getMainQueue() {
  if ((mainQueue = mq_open(QUEUE_NAME, O_WRONLY)) == -1) errorHandler(1);
}

void createQueue() {
  struct mq_attr attr;
  attr.mq_maxmsg = MAX_CLIENTS;
  attr.mq_msgsize = MAX_SIZE_OF_MESSAGE;

  giveQueueName();
  if ((myQueue = mq_open(clientQueueName, O_CREAT | O_RDWR, 0600, &attr)) == -1) errorHandler(1);
}

void giveQueueName() {
  clientQueueName[0] = '/';
  clientQueueName[1] = 'q';
  for (int i = 2 ; i < 13 ; i++) {
    clientQueueName[i] = (char)(rand()%61+61);
  }
  clientQueueName[13] = '\0';
}

void runningConnection() {
  printf("Klient zaczal dzialac!\n");

  Message message;

  message = loggingToServer(message);
  for (;;) {
    message = clientWriteIn(message);
    sendMessage(mainQueue, message.type, message.communique);
    if (mq_receive(myQueue, (char *)&message, MAX_LENGHT_OF_COMMUNIQUE, 0) == -1) errorHandler(2);
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

Message loggingToServer(Message message) {
  sendMessage(mainQueue, LOGOWANIE, clientQueueName);
  if (mq_receive(myQueue, (char *)&message, MAX_LENGHT_OF_COMMUNIQUE, 0) == -1) errorHandler(2);
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

Message sendMessage(mqd_t clientQueue, long typeOfMessage, char * textMessage) {
  printf("Wysylam wiadomosc.\n");

  Message response;
  response.type = typeOfMessage;
  response.senderPID = getpid();
  strcpy(response.communique, textMessage);

  if (mq_send(clientQueue, (char *)&response, MAX_LENGHT_OF_COMMUNIQUE, 1) == -1) errorHandler(3);

  return response;
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
  printf("Moje ID to: %d.\n", atoi(message.communique));
}

void serviceEcho(struct Message message) {
  printf("Otrzymalem od serwera: %s", message.communique);
}

void serviceToCapital(struct Message message) {
  printf("Otrzymalem od serwera: %s", message.communique);
}

void serviceTime(struct Message message) {
  printf("Otrzymalem od serwera: %s.\n", message.communique);
}

void serviceExitServer(struct Message message) {
  closeQueue();
  exit(0);
}

void closeQueue() {
  mq_close(myQueue);
  mq_unlink(clientQueueName);
}

void signalHandler(int signalType) {
  switch (signalType) {
    case SIGUSR1:
      closeQueue();
      printf("Zakonczylem dzialanie przez zlecenie NAKAZ_ZAMKNIECIA.\n");
      exit(0);
      break;
    case SIGTSTP:
      sendMessage(mainQueue, ZAMYKANIE_KOLEJKI, clientQueueName);
      closeQueue();
      exit(0);
      break;
    default: break;
  }
}

void errorHandler(int i) {
  switch (i) {
    case 1: printf("ERROR! Niepoprawne wykonanie polecenia mq_open!\n"); break;
    case 2: printf("ERROR! Niepoprawne wykonanie polecenia mq_receive! %d: %s\n",errno,strerror(errno)); break;
    case 3: printf("ERROR! Niepoprawne wykonanie polecenia mq_send!\n"); break;
    case 6: printf("ERROR! Odmowa zalogowania!\n"); break;
    default: printf("Brak takiego bledu!\n"); break;
  }
  sendMessage(mainQueue, ZAMYKANIE_KOLEJKI, clientQueueName);
  closeQueue();
  printf("Wylaczanie klienta z bledem.\n");
  exit(1);
}
