#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>

void exitOrError(int i) {
  switch (i) {
    case 1: printf("ERROR! Niepoprawna ilosc argumentow! Potrzeba dwoch. Sciezka do potoku nazwanego i liczba calkowita R.\n"); break;
    case 2: printf("ERROR! Argument numer 2 jest niepoprawny! Powinna to byc liczba calkowita dodatnia.\n"); break;
    default: printf("Brak takiego bledu!\n"); break;
  }
  exit(1);
}

void writeIndexToFile(int ** T, int R) {
  FILE * f = fopen("data","w");

  for (int i = 0 ; i < R ; i++) {
    for (int j = 0 ; j < R ; j++) fprintf(f,"%d %d %d\n",i,j,T[i][j]);
  }
  fclose(f);
}

void writePicture(int R) {
  FILE * picture = popen("gnuplot", "w");

  fprintf(picture, "set view map\n");
  fprintf(picture, "set xrange [0:%d]\n",R-1);
  fprintf(picture, "set yrange [0:%d]\n",R-1);
  fprintf(picture, "plot 'data' with image\n");
  fflush(picture);
  getc(stdin);
  pclose(picture);
}

void drawPicture(int ** T, int R) {
  writeIndexToFile(T, R);
  writePicture(R);
}

void scaleIndex(double * i, double * j, int R) {
  *i = R * fabs(-2 - *i) / 3.0;
  *j = R * fabs(1 - *j) / 2.0;
}

int ** setPicture(int FIFOfile, int ** T, int R) {
  size_t FIFOlineSize = 48;
  char buffer[FIFOlineSize];
  double firstIndex,secondIndex;
  int value;

  while (read(FIFOfile, buffer, FIFOlineSize) > 0) {
      sscanf(buffer, "%lf %lf %d", &firstIndex, &secondIndex, &value);
      scaleIndex(&firstIndex, &secondIndex, R);
      T[(int)firstIndex][(int)secondIndex] = value;
  }

  return T;
}

int ** preparePicture(char * path, int R, int ** T) {
  int FIFOfile = open(path, O_RDONLY);

  T = setPicture(FIFOfile, T, R);

  close(FIFOfile);
  unlink(path);

  return T;
}

int ** initPicture(int R, int ** T) {
  for (int i = 0 ; i < R; i++){
    T[i] = calloc((size_t)R,sizeof(int));
  }

  return T;
}

void writeSet(char * path, int R) {
  int ** T = malloc(sizeof(int *) * R);

  T = initPicture(R, T);
  T = preparePicture(path, R, T);
  drawPicture(T, R);

  free(T);
}

int main(int argc, char ** argv) {
  int R;
  char * path;

  if (argc != 3) exitOrError(1);
  if ((R = atoi(argv[2])) <=0 ) exitOrError(2);
  path = argv[1];
  mkfifo(path,0666);
  writeSet(path,R);

  return 0;
}
