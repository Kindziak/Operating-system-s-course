#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int parentPID;
time_t timeStarts;
time_t timeEnds;

void handlerFunction(int typeOfSignal){
    switch(typeOfSignal){
        case SIGUSR1:
            printf("Odebrano sygnal SIGUSR1 jako pozwolenie.\n");
            kill(parentPID,SIGRTMIN+rand()%33);
            time(&timeEnds);
            printf("Dla procesu: %d roznica czasu pomiedzy wyslaniem pierwszego sygnalu, a otrzymaniem pozwolenia to: %.2f sekund.\n",getpid(),difftime(timeEnds,timeStarts));
            printf("Zakonczono prace %i.\n",getpid());
            exit(0);
            //break;
        case SIGINT:
            exit(1);
        default:
            break;
    }
}

int main(){
  printf("Nowy potomek: %i.\n",getpid());
  srand((unsigned int) getpid());
  if(signal(SIGUSR1,handlerFunction)==SIG_ERR){
    printf("Blad podczas lapania sygnalu SIGUSR1.\n");
    exit(1);
  }
  if(signal(SIGINT,handlerFunction)==SIG_ERR){
    printf("Blad podczas lapania sygnalu SIGINT.\n");
    exit(1);
  }
  parentPID=getppid();
  time(&timeStarts);
  sleep(rand()%10);
  //sleep(10);
  printf("Wyslano SIGUSR1 z: %i.\n",getpid());
  //usleep(10);
  kill(parentPID,SIGUSR1);
  //sleep(1);
  printf("Oczekuje: %i.\n",getpid());
  //sleep(1);
  pause();
  //printf("Zakonczono prace %i.\n",getpid());
  //exit(0);
}
