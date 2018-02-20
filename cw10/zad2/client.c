#include "client.h"

char clientName[MAX_LENGHT_OF_NAME];
char * addressIPv4;
char * pathUNIX;
int portNumber, typeOfConnection; //0 siec, 1 lokalnie
int clientDescriptor = -1;
struct sockaddr_un sockUn,sockUn2;
char sockName2[MAX_LENGHT_OF_NAME];

int main(int argc, char ** argv) {
  signal(SIGINT,signalHandler);
  initializeArguments(argc,argv);
  makeConnection();
  //printf("ji\n");
  work();

  return 0;
}

void work() {
  char buffer[1024];
  Commission message;

  for(;;) {
    printf("ji\n");
    recvfrom(clientDescriptor,buffer,sizeof buffer, 0 , NULL, NULL);
    printf("%s\n",buffer);
    message = *(Commission*)buffer;
    if (message.operation == DODAJ || message.operation == ODEJMIJ || message.operation == POMNOZ || message.operation == PODZIEL) doCalculations(&message);
    sendto(clientDescriptor,(void *)&message, sizeof message, 0, (struct sockaddr *)&sockUn,sizeof sockUn);
  }
}

void doCalculations(Commission * message) {
  switch (message -> operation) {
    case DODAJ:
      message -> result = message -> argument1 + message -> argument2;
      break;
    case ODEJMIJ:
      message -> result = message -> argument1 - message -> argument2;
      break;
    case POMNOZ:
      message -> result = message -> argument1 * message -> argument2;
      break;
    case PODZIEL:
      message -> result = message -> argument1 / message -> argument2;
      break;
    default: break;
  }
  printf("Klient %s obliczyl wyrazenie %d %c %d = %d\n",message -> nameOfClient,message -> argument1,enumTOChar(message -> operation),message -> argument2,message -> result);
}

char enumTOChar(TypeOfCommission type) {
  for (int i = 0 ; i < sizeof(conversionTableForOperators) / sizeof(conversionTableForOperators[0]); i++) {
    if (conversionTableForOperators[i].typeAsEnum == type) return conversionTableForOperators[i].typeAsChar;
  }
  return '=';
}

void connectNetwork() {
  struct sockaddr_in sockIn;
  if ((clientDescriptor = socket(AF_INET,SOCK_STREAM,0)) < 0) errorHandler(2);
  sockIn.sin_family = AF_INET;
  sockIn.sin_port = htons(portNumber);
  if (inet_pton(AF_INET,addressIPv4,&sockIn.sin_addr) <= 0) errorHandler(3);
  if (connect(clientDescriptor,(struct sockaddr*)&sockIn,sizeof(struct sockaddr_in)) < 0) errorHandler(4);
}

void connectLocal() {
  memset(&sockUn2,0,sizeof sockUn2);
  if ((clientDescriptor = socket(AF_UNIX,SOCK_DGRAM,0)) < 0) errorHandler(2);
  sockUn2.sun_family = AF_UNIX;
  strcpy(sockUn2.sun_path, clientName);
  bind(clientDescriptor,(struct sockaddr *)&sockUn2,sizeof(struct sockaddr_un));

  memset(&sockUn2, 0, sizeof sockUn);
  sockUn.sun_family = AF_UNIX;
  strcpy(sockUn.sun_path,pathUNIX);
//  strcpy(sockUn2.sun_path,)
  //if (connect(clientDescriptor,(struct sockaddr*)&sockUn,sizeof(struct sockaddr_un)) < 0) errorHandler(4);
}

enum TypeOfCommission sendName() {
  char buffer[70] = {'\0'};
  Commission message;

  message.operation = NAZWA;
  strcpy(message.nameOfClient,clientName);
  printf("%s\n",message.nameOfClient);
  sendto(clientDescriptor,(void *)&message, sizeof message, 0, (struct sockaddr *)&sockUn,sizeof sockUn);
  //write(clientDescriptor,(void*)&message,sizeof(Commission));
  //printf("%s\n",message.nameOfClient);
  //read(clientDescriptor,buffer,sizeof(Commission));
  //printf("%s\n",buffer);
  recvfrom(clientDescriptor,buffer,sizeof buffer, 0 , NULL,NULL);
  printf("hi\n");
  printf("%s\n",buffer);
  message = *(Commission*)buffer;
  printf("%s\n",buffer);
  return message.operation;
}

void makeConnection() {
  if (typeOfConnection == 0) connectNetwork();
  else connectLocal();
  if (sendName() == AKCEPTACJA) printf("Klient zalogowany.\n");
  else {
    printf("Klient nei moze sie zalogowac.\n");
    exit(1);
  }
}

void initializeArguments(int argc, char** argv) {
  if (argc < 4 || argc > 5) errorHandler(1);
  strcpy(clientName,argv[1]);
  if (strcmp("lokalnie",argv[2]) == 0) {
    pathUNIX = malloc(strlen(argv[3])+1);
    strcpy(pathUNIX,argv[3]);
    typeOfConnection = 1;
  }
  else if (strcmp("siec",argv[2]) == 0) {
    addressIPv4 = malloc(strlen(argv[3])+1);
    strcpy(addressIPv4,argv[3]);
    portNumber = atoi(argv[4]);
    typeOfConnection = 0;
  }
  else errorHandler(1);
}

void signalHandler(int signalType) {
  Commission message;
  switch (signalType) {
    case SIGINT:
      message.operation = WYJSCIE;
      write(clientDescriptor,(void*)&message,sizeof(Commission));
      shutdown(clientDescriptor,SHUT_RDWR);
      exit(0);
      break;
    default: break;
  }
}

void errorHandler(int number) {
  switch (number) {
    case 1: printf("ERROR! Niepoprawna ilosc argumentow! Powinny byc 3 lub 4: nazwa klienta, sposob polaczenie z serwerem (siec lub lokalnie), adres i numer portu lub sciezke.\n"); exit(1);
    case 2: printf("ERROR! Niepoprawny argument! Ilosc watkow musi byc liczba calkowita dodatnia.\n"); exit(2);
    case 3: printf("ERROR! Niepoprawny argument! Ilosc rekordow czytanych przez watek w pojedynczym odczycie musi byc liczba calkowita dodatnia.\n"); exit(3);
    case 4: printf("ERROR! Niepoprawne wykonanie polecenia:!\n"); exit(4);
    //case 5: printf("ERROR! Niepoprawne wykonanie polecenia: pthread_create!\n"); exit(5);
    //case 6: printf("ERROR! Niepoprawne wykonanie polecenia: pthread_join!\n"); exit(6);
    //case 7: printf("ERROR! Niepoprawne wykonanie polecenia: pthread_setcanceltype!\n"); exit(7);
    //case 8: printf("ERROR! Niepoprawne wykonanie polecenia: read!\n"); exit(8);
    //case 9: printf("ERROR! Niepoprawne wykonanie polecenia: close!\n"); exit(9);
    default: printf("Brak takiego bledu!\n"); exit(0);
  }
}
