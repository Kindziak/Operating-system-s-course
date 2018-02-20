#ifndef _COMMON_H_
#define _COMMON_H_

#define KEY_GENERATE_NUMBER 20
#define MAX_CLIENTS 10
#define MAX_LENGHT_OF_COMMUNIQUE 32
#define MAIN_QUEUE_PATH "HOME"
#define QUEUE_NAME "/server"

typedef enum TypeOfCommission {
  LOGOWANIE = 1, ECHO = 2, DUZA_LITERA = 3, CZAS = 4, NAKAZ_ZAMKNIECIA = 5, NIE = 6, AKCEPTACJA = 7, ODMOWA = 8, ZAMYKANIE_KOLEJKI = 9
} TypeOfCommission;

const static struct ConversionTable {
  TypeOfCommission typeAsEnum;
  const char * typeAsString;
} conversionTable[] = { {LOGOWANIE, "LOGOWANIE"}, {ECHO, "ECHO"}, {DUZA_LITERA, "DUZA_LITERA"}, {CZAS, "CZAS"}, {NAKAZ_ZAMKNIECIA, "NAKAZ_ZAMKNIECIA"}, {NIE, "NIE"}, {AKCEPTACJA, "AKCEPTACJA"}, {ODMOWA, "ODMOWA"}, {ZAMYKANIE_KOLEJKI, "ZAMYKANIE_KOLEJKI"} };

typedef struct Message {
  long type;
  pid_t senderPID;
  char communique[MAX_LENGHT_OF_COMMUNIQUE];
} Message;

static const size_t MAX_SIZE_OF_MESSAGE = sizeof(char) * MAX_LENGHT_OF_COMMUNIQUE;

#endif
