#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

int * children;
int * waitingForPermition;
int numberOfSignals=0;
int n,k;
int ended=0;

int isInQueue(int pid){
  for(int i=0 ; i<k ; i++) if(waitingForPermition[i]==pid) return 1;
  return 0;
}

void addToQueue(int pid){
    int i=0;
    for(i ; i<n ; i++){
      if(waitingForPermition[i]==(-1)){
        //numberOfSignals++;
        waitingForPermition[i]=pid;
        break;
      }
    }
}

void handlerFunction(int typeOfSignal, siginfo_t * infoAct, void * context){
    switch(typeOfSignal){
        case SIGUSR1:
            printf("Glowny proces odebral sygnal SIGUSR1 od: %i.\n",(*infoAct).si_pid);
            if(!isInQueue((*infoAct).si_pid)) addToQueue((*infoAct).si_pid);
            if(numberOfSignals==k) kill((*infoAct).si_pid,SIGUSR1);
            else{
              numberOfSignals++;
              if(numberOfSignals==k){
                for(int i=0 ; i<n ; i++){
                  if(waitingForPermition[i]!=(-1)){
                    kill(waitingForPermition[i],SIGUSR1);
                    //wait((pid_t)waitingForPermition[i]);
                    //wait(0);
                    //pause();
                    waitpid(waitingForPermition[i],NULL,0);
                  }
                }
              }
            }
            break;
        case SIGINT:
            for(int i=0 ; i<n ; i++) kill(children[i],SIGINT);
            exit(1);
        case SIGCHLD:
            ended++;
            printf("Proces glowny otrzymal potwierdzenie zakonczenia procesu potomnego: %i z kodem: %i.\n",(*infoAct).si_pid,(*infoAct).si_status);
            //printf("%i\n",ended);
            break;
        default:
            break;
    }
    if(typeOfSignal<=SIGRTMAX && typeOfSignal>=SIGRTMIN) printf("Proces glowny otrzymal sygnal czasu rzeczywistego.\n");
}

int main(int argc, char ** argv){
  if(argc!=3){
    printf("Zla ilosc argumentow! Potrzebne sa dwa - ilosc procesow do stworzenia i ilosc sygnalow.\n");
    exit(1);
  }
  if(atoi(argv[1])<=0 && atoi(argv[2])<=0){
  	printf("Argumenty musza byc liczbami naturalnymi wiekszymi od 0.\n");
  	exit(1);
  }
  n=atoi(argv[1]);
  k=atoi(argv[2]);
  struct sigaction act;
  act.sa_flags=SA_SIGINFO;
  act.sa_sigaction=&handlerFunction;
  sigemptyset(&act.sa_mask);
  sigaction(SIGUSR1,&act,NULL);
  sigaction(SIGINT,&act,NULL);
  sigaction(SIGCHLD, &act,NULL);
  for(int i=SIGRTMIN ; i<=SIGRTMAX ; i++) sigaction(i,&act,NULL);
  waitingForPermition=calloc((size_t)n,sizeof(int *));
  for(int i=0 ; i<n ; i++) waitingForPermition[i]=-1;
  children=calloc((size_t)n,sizeof(int *));
  for(int i=0 ; i<n ; i++) children[i]=-1;
  int tmp=1;
  for(int i=0 ; i<n ; i++){
    if(tmp!=0){
      tmp=fork();
      children[i]=tmp;
    }
  }
  if(tmp<0){
    printf("Blad podczas tworzenia potomka.\n");
    exit(1);
  }
  else if(tmp==0){
    assert(execl("children.run","children.run",NULL)>=0);
  }
  else{
    while(wait(NULL)){
      if(ended>=n) break;
    }
  }
  return 0;
}
