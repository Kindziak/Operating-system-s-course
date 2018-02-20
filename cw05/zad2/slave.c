#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <complex.h>
#include <time.h>

void exitOrError(int i) {
  switch (i) {
    case 1: printf("ERROR! Niepoprawna ilosc argumentow! Potrzeba trzech. Sciezka do potoku nazwanego i liczba calkowita R.\n"); break;
    case 2: printf("ERROR! Argument numer 2 lub 3 jest niepoprawny! Powinna to byc liczba calkowita dodatnia.\n"); break;
    default: printf("Brak takiego bledu!\n"); break;
  }
  exit(1);
}

double randomNumber(int i) {
  double result = 0;

  if (i = 1) result = (-2) * (double)rand() / (double)RAND_MAX + (double)rand() / (double)RAND_MAX;
  else result = (-1) * (double)rand() / (double)RAND_MAX + (double)rand() / (double)RAND_MAX;

  return result;
}

void setFIFO(int FIFOfile, int N, int K) {
  size_t FIFOlineSize = 48;
  int iterationsNumber;
  double complex c, z0;
  char buffer[FIFOlineSize];
  for (int i = 0 ; i < N ; i++) {
    c = randomNumber(1) + randomNumber(2) * i;
    z0 = 0;
    for (iterationsNumber = 0 ; cabs(z0) <= 2 && iterationsNumber < K ; iterationsNumber++) z0 = z0 * z0 + c;
    sprintf(buffer, "%lf %lf %d", creal(c), cimag(c), iterationsNumber);
    write(FIFOfile, buffer, FIFOlineSize);
  }
}

void writeToFIFO(char * path, int N, int K) {
  int FIFOfile = open(path, S_IFIFO | O_WRONLY);

  setFIFO(FIFOfile, N, K);
  close(FIFOfile);
}

int main(int argc, char ** argv) {
  srand((unsigned int)(time(NULL)));
  int N, K;
  char * path;

  sleep(1);
  if (argc != 4) exitOrError(1);
  if ((N = atoi(argv[2])) <= 0 || (K = atoi(argv[3]) <= 0)) exitOrError(2);
  path = argv[1];

  writeToFIFO(path, N, K);

  return 0;
}
