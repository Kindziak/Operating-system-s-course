#ifndef _COMMON_H_
#define _COMMON_H_

#define MAX_CLIENTS 10
#define MAX_LENGHT_OF_NAME 16

typedef enum TypeOfCommission {
  DODAJ = 1, ODEJMIJ = 2, POMNOZ = 3, PODZIEL = 4, PING = 5, NAZWA = 6, WYJSCIE = 7, AKCEPTACJA = 8, ODMOWA = 9
} TypeOfCommission;

const static struct ConversionTable {
  TypeOfCommission typeAsEnum;
  const char * typeAsString;
} conversionTable[] = { {DODAJ, "DODAJ"}, {ODEJMIJ, "ODEJMIJ"}, {POMNOZ, "POMNOZ"}, {PODZIEL, "PODZIEL"}, {PING, "PING"}, {NAZWA, "NAZWA"}, {WYJSCIE, "WYJSCIE"}, {AKCEPTACJA, "AKCEPTACJA"}, {ODMOWA, "ODMOWA"} };

const static struct ConversionTableForOperators {
  TypeOfCommission typeAsEnum;
  const char typeAsChar;
} conversionTableForOperators[] = { {DODAJ, '+'}, {ODEJMIJ, '-'}, {POMNOZ, '*'}, {PODZIEL, '/'} };

typedef struct Commission {
  char nameOfClient[MAX_LENGHT_OF_NAME];
  TypeOfCommission operation;
  int argument1;
  int argument2;
  int result;
} Commission;

typedef struct Client {
  char nameOfClient[MAX_LENGHT_OF_NAME];
  int descriptor;
} Client;

#endif
