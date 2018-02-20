#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

void clockStarts(){
  st_time = times(&st_cpu);
}

void clockEnds(double * t){
  static long clock = 0;
  clock = sysconf(_SC_CLK_TCK);
  en_time = times(&en_cpu);
  t[0] = (double)(en_time - st_time) / (double)clock;
  t[1] = (double)(en_cpu.tms_utime - st_cpu.tms_utime) / (double)clock;
  t[2] = (double)(en_cpu.tms_stime - st_cpu.tms_stime) / (double)clock;
  printf("REAL TIME: %4.2f, USER TIME: %4.2f, SYSTEM TIME: %4.2f \n", t[0], t[1], t[2]);
}

void generate(char * fileName, int sizeOfRecord, int numberOfRecords){
  char * bufor = calloc(sizeOfRecord, numberOfRecords);
  int n = sizeOfRecord * numberOfRecords;

  int file = open(fileName, O_CREAT | O_RDWR | O_APPEND | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  if(file==-1){
    printf("Blad w otwieraniu pliku. \n");
    exit(1);
  }
  int randomData = open("/dev/urandom", O_RDONLY);
  if(randomData==-1){
    printf("Blad w otwieraniu pliku. \n");
    exit(1);
  }

  int result = read(randomData,bufor,n);
  if(result!=n){
    printf("Blad w odczytywaniu pliku. \n");
    exit(1);
  }

  result = write(file,bufor,n);
  if(result!=n){
    printf("Blad w zapisywaniu pliku. \n");
    exit(1);
  }

  if(close(file)==-1 || close(randomData)==-1){
    printf("Blad w zamykaniu pliku. \n");
  }
}

void readSYS(char * fileName, char * record, int sizeOfRecord, int position){
  int file = open(fileName,O_RDWR);
  if(lseek(file,position,SEEK_SET)<0) exit(1);
  if(read(file,record,sizeOfRecord)<0){
    printf("Blad podczas czytania. \n");
    exit(1);
  }
  if(close(file)!=0){
    printf("Blad w zamykaniu pliku. \n");
    exit(1);
  }
}

void writeSYS(char * fileName, char * record, int sizeOfRecord, int position){
  int file = open(fileName,O_RDWR);
  if(lseek(file,position,SEEK_SET)<0) exit(1);
  if(write(file,record,sizeOfRecord)<0){
    printf("Blad podczas zapisywania. \n");
    exit(1);
  }
  if(close(file)!=0){
    printf("Blad w zamykaniu pliku. \n");
    exit(1);
  }
}

void shuffleSYS(char * fileName, int sizeOfRecord, int numberOfRecords){
  char * record1 = calloc(1,sizeOfRecord);
  char * record2 = calloc(1,sizeOfRecord);
  for(int i=numberOfRecords-1 ; i>0 ; i--){
    int j = rand()%i;
    readSYS(fileName,record1,sizeOfRecord,i*sizeOfRecord);
    readSYS(fileName,record2,sizeOfRecord,j*sizeOfRecord);
    writeSYS(fileName,record1,sizeOfRecord,j*sizeOfRecord);
    writeSYS(fileName,record2,sizeOfRecord,i*sizeOfRecord);
  }
}

void sortSYS(char * fileName, int sizeOfRecord, int numberOfRecords){
  char * record1 = (char *)malloc(sizeOfRecord * sizeof(char));
  char * record2 = (char *)malloc(sizeOfRecord * sizeof(char));
  for(int i=0 ; i<numberOfRecords ; i++){
    for(int j=0 ; j<numberOfRecords-1-i ; j++){
      readSYS(fileName,record1,sizeOfRecord,i*sizeOfRecord);
      readSYS(fileName,record2,sizeOfRecord,j*sizeOfRecord);
      if(strncmp(record1,record2,1)<0){
        writeSYS(fileName,record1,sizeOfRecord,j*sizeOfRecord);
        writeSYS(fileName,record2,sizeOfRecord,i*sizeOfRecord);
      }
    }
  }

  /*int wasSwitch = 1;
  while(wasSwitch){
    printf("lol");
    wasSwitch = 0;
    for(int i=0 ; i<numberOfRecords ; i++){
      readSYS(fileName,record1,sizeOfRecord,i*sizeOfRecord);
      readSYS(fileName,record2,sizeOfRecord,(i+1)*sizeOfRecord);
      if(record1[0]<record2[0]){
        wasSwitch = 1;
        writeSYS(fileName,record1,sizeOfRecord,(i+1)*sizeOfRecord);
        writeSYS(fileName,record2,sizeOfRecord,i*sizeOfRecord);
      }
    }
  }*/
}

void readLIB(char * fileName, char * record, int sizeOfRecord, int position){
  FILE * file = fopen(fileName,"r+");
  fseek(file,position,SEEK_SET);
  if(fread(record,1,sizeOfRecord,file)!=sizeOfRecord){
    printf("Blad podczas czytania. \n");
    exit(1);
  }
  fflush(file);
  if(fclose(file)!=0){
    printf("Blad w zamykaniu pliku. \n");
    exit(1);
  }
}

void writeLIB(char * fileName, char * record, int sizeOfRecord, int position){
  FILE * file = fopen(fileName,"r+");
  fseek(file,position,SEEK_SET);
  if(fwrite(record,1,sizeOfRecord,file)!=sizeOfRecord){
    printf("Blad podczas wpisywania. \n");
    exit(1);
  }
  fflush(file);
  if(fclose(file)!=0){
    printf("Blad w zamykaniu pliku. \n");
    exit(1);
  }
}

void shuffleLIB(char * fileName, int sizeOfRecord, int numberOfRecords){
  char * record1 = calloc(1,sizeOfRecord);
  char * record2 = calloc(1,sizeOfRecord);
  for(int i=numberOfRecords-1 ; i>0 ; i--){
    int j = rand()%i;
    readLIB(fileName,record1,sizeOfRecord,i*sizeOfRecord);
    readLIB(fileName,record2,sizeOfRecord,j*sizeOfRecord);
    writeLIB(fileName,record1,sizeOfRecord,j*sizeOfRecord);
    writeLIB(fileName,record2,sizeOfRecord,i*sizeOfRecord);
  }
}

void sortLIB(char * fileName, int sizeOfRecord, int numberOfRecords){
  char * record1 = (char *)malloc(sizeOfRecord * sizeof(char));
  char * record2 = (char *)malloc(sizeOfRecord * sizeof(char));
  for(int i=0 ; i<numberOfRecords ; i++){
    for(int j=0 ; j<numberOfRecords-1-i ; j++){
      readLIB(fileName,record1,sizeOfRecord,i*sizeOfRecord);
      readLIB(fileName,record2,sizeOfRecord,j*sizeOfRecord);
      if(strncmp(record1,record2,1)<0){
        writeLIB(fileName,record1,sizeOfRecord,j*sizeOfRecord);
        writeLIB(fileName,record2,sizeOfRecord,i*sizeOfRecord);
      }
    }
  }
}

void copy(char * fileName1, char * fileName2, int sizeOfRecord, int numberOfRecords){
  FILE * file1 = fopen(fileName1, "r");
  if(file1 == NULL){
    printf("Nie znaleziono pliku.\n");
    exit(1);
  }
  FILE * file2 = fopen(fileName2, "w");
  if(file2 == NULL){
    printf("Nie znaleziono pliku.\n");
    exit(1);
  }
  char * record = (char *)malloc(sizeOfRecord * sizeof(char));
  for(int i=0 ; i<numberOfRecords ; ++i){
    readLIB(fileName1,record,sizeOfRecord,i*sizeOfRecord);
    writeLIB(fileName2,record,sizeOfRecord,i*sizeOfRecord);
  }
}

int main(int argc, char ** argv){
  srand(time(NULL));
  double * t1 = (double*)malloc(3*sizeof(double));
  char * fileName = (char *)malloc(256 * sizeof(char));

  if(argc !=6){
    printf("Zla liczba argumentow. Ma ich byc 5.");
    exit(1);
  }

  fileName = argv[3];
  int sizeOfRecord = atoi(argv[5]);
  int numberOfRecords = atoi(argv[4]);

  if(sizeOfRecord<=0 || numberOfRecords<=0){
    printf("Argumenty Liczbowe (nr 4 i 5) musza byc wieksze od 0.\n");
    exit(1);
  }

  generate(fileName,sizeOfRecord,numberOfRecords);
  copy(fileName,"copy",sizeOfRecord,numberOfRecords);

  if(strcmp(argv[2],"shuffle") == 0){
    if(strcmp(argv[1],"sys") == 0){
      printf("Shuffle dla SYS.\n");
      clockStarts();
      shuffleSYS(fileName,sizeOfRecord,numberOfRecords);
      clockEnds(t1);
    }
    else if(strcmp(argv[1],"lib") == 0){
      printf("Shuffle dla LIB.\n");
      clockStarts();
      shuffleLIB(fileName,sizeOfRecord,numberOfRecords);
      clockEnds(t1);
    }
    else{
      printf("Zly argument nr 1.");
      exit(1);
    }
  }
  else if(strcmp(argv[2],"sort") == 0){
    if(strcmp(argv[1],"sys") == 0){
      printf("Sort dla SYS.\n");
      clockStarts();
      sortSYS(fileName,sizeOfRecord,numberOfRecords);
      clockEnds(t1);
    }
    else if(strcmp(argv[1],"lib") == 0){
      printf("Sort dla LIB.\n");
      clockStarts();
      sortLIB(fileName,sizeOfRecord,numberOfRecords);
      clockEnds(t1);
    }
    else{
      printf("Zly argument nr 1.\n");
      exit(1);
    }
  }
  else{
    printf("Zly argument nr 2.\n");
    exit(1);
  }
  return 0;
}
