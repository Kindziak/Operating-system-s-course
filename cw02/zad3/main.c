#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void setLock(int fileDescriptor, int bajtNumber, int type, int bORn ){
  struct flock * lock = (struct flock *)malloc(sizeof(struct flock));
  if(type==0){
    lock->l_whence = SEEK_SET;
    lock->l_len = 1;
    lock->l_start = bajtNumber;
    lock->l_type = F_WRLCK;
  }
  else{
    lock->l_whence = SEEK_SET;
    lock->l_len = 1;
    lock->l_start = bajtNumber;
    lock->l_type = F_RDLCK;
  }
  if(bORn==0){
    if(fcntl(fileDescriptor,F_SETLK,lock)==-1){
      //free(lock);
      perror("file");
      return;
    }
    printf("Rygiel ustawiony.\n");
    //free(lock);
  }
  else{
    if(fcntl(fileDescriptor,F_SETLKW,lock)==-1){
      //free(lock);
      perror("file");
      return;
    }
    printf("Rygiel ustawiony.\n");
    //free(lock);
  }
}

void displayLocks(int fileDescriptor){
  long int fileSize = lseek(fileDescriptor,0,SEEK_END);
  int iterator = 0;
  struct flock * lock = (struct flock *)malloc(sizeof(struct flock));
  while(lseek(fileDescriptor,iterator,SEEK_SET)!=fileSize){
    lock->l_type = F_RDLCK;
    lock->l_whence = SEEK_SET;
    lock->l_len = 1;
    lock->l_start = iterator;
    fcntl(fileDescriptor,F_GETLK,lock);
    if(lock->l_type!=F_UNLCK) printf("Typ: odczyt , Znak: %ld , PID: %d \n",iterator,lock->l_pid);
    iterator++;
  }
  iterator = 0;
  while(lseek(fileDescriptor,iterator,SEEK_SET)!=fileSize){
    lock->l_type = F_WRLCK;
    lock->l_whence = SEEK_SET;
    lock->l_len = 1;
    lock->l_start = iterator;
    fcntl(fileDescriptor,F_GETLK,lock);
    if(lock->l_type!=F_UNLCK) printf("Typ: zapis , Znak: %ld , PID: %d \n",iterator,lock->l_pid);
    iterator++;
  }
  free(lock);
}

void freeLock(int fileDescriptor, int bajtNumber){
  struct flock * lock = (struct flock *)malloc(sizeof(struct flock));
  lock->l_type = F_UNLCK;
  lock->l_whence = SEEK_SET;
  lock->l_len = 1;
  lock->l_start = bajtNumber;

  if(fcntl(fileDescriptor,F_SETLK,lock)==-1){
    free(lock);
    perror("file");
    return;
  }
  printf("Rygiel zwolniony.\n");
  free(lock);
}

void readChar(int fileDescriptor, int bajtNumber){
  char x[2];
  lseek(fileDescriptor,bajtNumber,SEEK_SET);
  read(fileDescriptor,&x,1);
  printf("Odczytany znak %c \n",x[0]);
}

void writeChar(int fileDescriptor, int bajtNumber, char x){
  lseek(fileDescriptor,bajtNumber,SEEK_SET);
  write(fileDescriptor,&x,1);
  printf("Zapisano znak.\n");
}

int main(int argc, char ** argv){
  char * fileName = (char *)malloc(256 * sizeof(char));
  int option,bajtNumber,ex=0;
  char x;

  if(argc !=2){
    printf("Zla liczba argumentow. Ma ich byc 1.");
    exit(1);
  }

  fileName = argv[1];

  int fileDescriptor = open(fileName, O_RDWR);
  if(fileDescriptor==-1){
    printf("Blad w otwieraniu pliku. \n");
    exit(1);
  }
  while(ex==0){
  printf("Dostepne opcje:\n");
  printf("1. Ustawienie rygla do odczytu na wybrany znak pliku. (nieblokujaca)\n");
  printf("11. Ustawienie rygla do odczytu na wybrany znak pliku. (blokujaca)\n");
  printf("2. Ustawienie rygla do zapisu na wybrany znak pliku. (nieblokujaca)\n");
  printf("22. Ustawienie rygla do zapisu na wybrany znak pliku. (blokujaca)\n");
  printf("3. Wyswieltenie lisyt zaryglowanych znakow pliku.\n");
  printf("4. Zwolnienie wybranego rygla.\n");
  printf("5. Odczyt wybranego znaku pliku.\n");
  printf("6. Zmaina wybranego znaku pliku.\n");
  printf("7. Wyjscie.\n");
  printf("Wpisz numer opcji, ktora wybierasz.\n");
  scanf("%d",&option);
  if(bajtNumber<0){
    printf("Numer bajtu musi byc dodatni.\n");
    exit(1);
  }
  switch(option){
    case 1:
      printf("Podaj numer bajtu w pliku.\n");
      scanf("%d",&bajtNumber);
      setLock(fileDescriptor,bajtNumber,1,0);
      break;
    case 11:
      printf("Podaj numer bajtu w pliku.\n");
      scanf("%d",&bajtNumber);
      setLock(fileDescriptor,bajtNumber,1,1);
      break;
    case 2:
      printf("Podaj numer bajtu w pliku.\n");
      scanf("%d",&bajtNumber);
      setLock(fileDescriptor,bajtNumber,0,0);
      break;
    case 22:
      printf("Podaj numer bajtu w pliku.\n");
      scanf("%d",&bajtNumber);
      setLock(fileDescriptor,bajtNumber,0,1);
      break;
    case 3:
      displayLocks(fileDescriptor);
      break;
    case 4:
      printf("Podaj numer bajtu w pliku.\n");
      scanf("%d",&bajtNumber);
      freeLock(fileDescriptor,bajtNumber);
      break;
    case 5:
      printf("Podaj numer bajtu w pliku.\n");
      scanf("%d",&bajtNumber);
      readChar(fileDescriptor,bajtNumber);
      break;
    case 6:
      printf("Podaj numer bajtu w pliku.\n");
      scanf("%d",&bajtNumber);
      printf("Podaj char do wpisania.\n");
      scanf("\n%c",&x);
      writeChar(fileDescriptor,bajtNumber,x);
      break;
    case 7:
      ex=1;
      break;
    default:
      printf("Wpisales bledna cyfre.\n");
      break;
  }
}
  close(fileDescriptor);
  //free(fileName);

  return 0;
}
