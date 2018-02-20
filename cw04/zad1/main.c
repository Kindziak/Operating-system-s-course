#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int lettersBack=0;

void handlerFunction(int typeOfSignal){
    switch(typeOfSignal){
        case SIGINT:
            printf("\nOdebrano sygnał SIGINT.\n");
            exit(0);
            break;
        case SIGTSTP:
            if(lettersBack==0) lettersBack=1;
            else lettersBack=0;
            printf("\n");
            break;
        default:
            break;
    }
}

int main(){
    signal(SIGINT,handlerFunction);
    struct sigaction act;
    int A=65;
    while(1){
      for( ; A<91 && lettersBack==0 ; A++){
          printf("%c\n",(char)A);
          act.sa_handler=handlerFunction;
          sigaction(SIGTSTP,&act,NULL);
          sleep(1);
          if(A==90) A=64;
      }
      for( ; A>64 && lettersBack==1 ; A--){
          printf("%c\n",(char)A);
          act.sa_handler=handlerFunction;
          sigaction(SIGTSTP,&act,NULL);
          sleep(1);
          if(A==65) A=91;
      }
      sleep(1);
    }
    return 0;
}
