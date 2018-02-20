#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

pid_t children;
int confirmation=0;
int numberOfSignals=0;
const union sigval val;

void handlerFunction(int typeOfSignal){
    switch(typeOfSignal){
        case SIGUSR1:
            numberOfSignals++;
            break;
        case SIGUSR2:
            printf("M: Odebrano sygnal SIGUSR2. Zakonczono prace procesu macierzystego. Odebrano %d sygnalow SIGUSR1 + 1 sygnal SIGUSR2.\n",numberOfSignals);
            exit(0);
        case SIGINT:
            kill(children,SIGKILL);
            printf("M: Odebrano sygnal SIGINT. Zakonczono prace sygnalu macierzystego i potomnego.\n");
            exit(1);
        default:
            break;
    }
    if(typeOfSignal==SIGRTMIN){
      numberOfSignals++;
    }
    if(typeOfSignal==SIGRTMAX){
      printf("M: Odebrano sygnal SIGRTMAX. Zakonczono prace procesu macierzystego. Odebrano %d sygnalow SIGUSR1 + 1 sygnal SIGURTMAX.\n",numberOfSignals);
      exit(0);
    }
}

int main(int argc, char ** argv){
  if(argc!=3){
    printf("Zla ilosc argumentow! Potrzebne sa dwa - ilosc SIGUSR1 do wyslania i typ.\n");
    exit(1);
  }
  int L=atoi(argv[1]);
  int Type=atoi(argv[2]);
  if(Type<=0 || Type>3){
    printf("Mamy 3 typy wysylania sygnalow.\n");
    exit(1);
  }
  if(signal(SIGUSR1,handlerFunction)==SIG_ERR){
    printf("Blad podczas lapania sygnalu SIGUSR1.\n");
    exit(1);
  }
  if(signal(SIGUSR2,handlerFunction)==SIG_ERR){
    printf("Blad podczas lapania sygnalu SIGUSR2.\n");
    exit(1);
  }
  if(signal(SIGINT,handlerFunction)==SIG_ERR){
    printf("Blad podczas lapania sygnalu SIGINT.\n");
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
  children=fork();
  if(children<0){
    printf("Blad podczas tworzenia potomka.\n");
    exit(1);
  }
  else if(children==0){
    if(execl("children.run","children.run",NULL)<0){
      printf("Blad potomka.\n");
      exit(1);
    }
  }
  printf("M: Wyslano %d sygnalow SIGUSR1 + 1 sygnal SIGUSR2.\n",L);
  sleep(1);
  switch(Type){
    case 1:
      for(int i=0 ; i<L ; i++){
        kill(children,SIGUSR1);
        //sleep(2);
      }
      kill(children,SIGUSR2);
      //sleep(2);
      break;
    case 2:
      for(int i=0 ; i<L ; i++){
        sigqueue(children,SIGUSR1,val);
      }
      sigqueue(children,SIGUSR2,val);
      break;
    case 3:
      for(int i=0 ; i<L ; i++){
        kill(children,SIGRTMIN);
      }
      kill(children,SIGRTMAX);
      break;
  }
  while(1){}
  return 0;
}
