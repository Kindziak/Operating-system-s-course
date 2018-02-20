#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int parentPID;
int numberOfReceivedSignals=0;

void handlerFunction(int typeOfSignal){
    switch(typeOfSignal){
        case SIGUSR1:
            numberOfReceivedSignals++;
            //printf("P: Odebrano sygnal SIGUSR1. Odeslano sygnal SIGUSR1.\n");
            kill(parentPID,SIGUSR1);
            break;
        case SIGUSR2:
            printf("P: Odebrano sygnal SIGUSR2. Zakonczono prace procesu. Odebrano %d sygnalow SIGUSR1 + 1 sygnal SIGUSR2.\n",numberOfReceivedSignals);
            kill(parentPID,SIGUSR2);
            exit(0);
        default:
            break;
    }
    if(typeOfSignal==SIGRTMIN){
      numberOfReceivedSignals++;
      //printf("P: Odebrano sygnal SIGRTMIN. Odeslano sygnal SIGRTMIN.\n");
      kill(parentPID,SIGRTMIN);
    }
    if(typeOfSignal==SIGRTMAX){
      printf("P: Odebrano sygnal SIGRTMAX. Zakonczono prace procesu. Odebrano %d sygnalow SIGRTMIN + 1 sygnal SIGRTMAX.\n",numberOfReceivedSignals);
      kill(parentPID,SIGRTMAX);
      exit(0);
    }
}

int main(){
  parentPID=getppid();
  if(signal(SIGUSR1,handlerFunction)==SIG_ERR){
    printf("Blad podczas lapania sygnalu SIGUSR1.\n");
    exit(1);
  }
  if(signal(SIGUSR2,handlerFunction)==SIG_ERR){
    printf("Blad podczas lapania sygnalu SIGUSR2.\n");
    exit(1);
  }
  if(signal(SIGRTMIN,handlerFunction)==SIG_ERR){
    printf("Blad podczas lapania sygnalu SIGRTMIN.\n");
    exit(1);
  }
  if(signal(SIGRTMAX,handlerFunction)==SIG_ERR){
    printf("Blad podczas lapania sygnalu SIGRTMAX.\n");
    exit(1);
  }
  while(1){}
  return 0;
}
