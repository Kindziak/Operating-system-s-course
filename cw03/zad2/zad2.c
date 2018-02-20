#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

static const int MAX=5;
pid_t children;

void interpret(FILE * file, int limitOfCpu, int limitOfMemory){
  char * nameOfVariable;
  char * valueOfVariable;
  char * charactersToOmit=" #\n";
  char * tableOfArg[MAX];
  char * line=NULL;
  size_t lengthOfLine=0;
  int check,check2;
  struct rlimit * limits;
  struct rusage rusages;
  float timeOfSubstract1=0;
	float timeOfSubstract2=0;
  while(getline(&line,&lengthOfLine,file)!=(-1)){
    if(line[0]=='\n') continue;
    if(line[0]=='#'){
      int iter=0;
  		while(line[iter]!='\0' && line[iter]!=' ') iter++;
  		if(line[iter]=='\0'){
				nameOfVariable=strtok(line,charactersToOmit);
				if(getenv(nameOfVariable)==NULL) printf("Zmienna %s nie moze zostac usunieta, poniewaz nie istnieje.\n",nameOfVariable);
				else{
					if(unsetenv(nameOfVariable)==-1) printf("Blad! Usuwanie zmiennej %s nie powiodlo sie.\n",nameOfVariable);
					else printf("Zmienna %s zostala usunieta.\n",nameOfVariable);
  			}
  		}
      else{
  			nameOfVariable=strtok(line,charactersToOmit);
  			valueOfVariable=strtok(NULL,charactersToOmit);
  			if(getenv(nameOfVariable)==NULL){
					if(setenv(nameOfVariable,valueOfVariable,0)==(-1)) printf("Blad! Ustawianie zmiennej %s nie powiodlo sie.\n",nameOfVariable);
					else printf("Zmienna %s zostala ustawiona.\n",nameOfVariable);
  			}
  			else{
  				if(setenv(nameOfVariable,valueOfVariable,1)==(-1)) printf("Blad! Ustawianie zmiennej %s nie powiodlo sie.\n",nameOfVariable);
          else printf("Zmienna %s zostala ustawiona.\n",nameOfVariable);
        }
			}
    }
    else{
      for(int k=MAX-1 ; k>=0 ; k--) tableOfArg[k]='\0';
			tableOfArg[0]=strtok(line,charactersToOmit);
  		nameOfVariable=tableOfArg[0];
      int i=1;
      while(i<MAX && (valueOfVariable=strtok(NULL,charactersToOmit))!=NULL){
        tableOfArg[i]=valueOfVariable;
        i++;
      }
  		children=fork();
      if(children<0){
        printf("Blad! Nie powiodl sie fork.\n");
        exit(1);
      }
      else if(children==0){
        limits=calloc(1,sizeof(struct rlimit));
				limits->rlim_max=(unsigned)limitOfMemory;
				limits->rlim_cur=(unsigned)limitOfMemory;
				check2=setrlimit(RLIMIT_AS,limits);
				free(limits);
				if(check2!= 0){
					printf("Blad! Ustawianie limitu na rozmiar pamieci nie powiodlo sie.\n");
					exit(1);
				}
				limits=calloc(1,sizeof(struct rlimit));
				limits->rlim_max=(unsigned)limitOfCpu;
				limits->rlim_cur=(unsigned)limitOfCpu;
				check2=setrlimit(RLIMIT_CPU,limits);
				free(limits);
				if(check2!=0){
					printf("Blad! Ustawianie limitu na czas dostepu nie powiodlo sie.\n");
					exit(1);
				}
        int checkChildren;
				checkChildren=execvp(nameOfVariable,tableOfArg);
				if (checkChildren==(-1)){
					printf("Blad! Nie powiodl sie execvp.\n");
					exit(1);
				}
			}
			else{
  			wait(&check);
  			if(!WIFEXITED(check)) printf("Proces %s zostal zakonczony bledem %d.\n",nameOfVariable,WEXITSTATUS(check));
				getrusage(RUSAGE_CHILDREN,&rusages);
				printf("Nazwa zmiennej: %s, Pamiec: %dMB, Czasy: -systemowy: %fs, -uzytkownika: %fs.\n",nameOfVariable,(int)rusages.ru_maxrss/1024,(float)rusages.ru_stime.tv_sec-timeOfSubstract1,(float)rusages.ru_utime.tv_sec-timeOfSubstract2);
        timeOfSubstract1=(float)rusages.ru_stime.tv_sec;
      	timeOfSubstract2=(float)rusages.ru_utime.tv_sec;
      }
    }
  }
}

int main(int argc, char ** argv){
  if(argc!=4){
    printf("Zla ilosc argumentow! Potrzebny sa trzy - sciezka do pliku, ograniczenie na dostepny czas procesora i rozmiar pamieci wirtualnej.\n");
    exit(1);
  }
  char * fileName=argv[1];
  if(atoi(argv[2])<0 || atoi(argv[3])<0){
		printf("Podane ograniczenia na czas dostepu i pamiec musza byc wieksze od 0.\n");
		exit(1);
	}
	int limitOfCpu=atoi(argv[2]);
	int limitOfMemory=atoi(argv[3]);
	limitOfMemory*=(1024*1024);
  FILE * file=fopen(fileName,"r");
	if(file==NULL){
		printf("Blad! Nie udalo otworzyc sie pliku.\n");
		exit(1);
	}
  interpret(file,limitOfCpu,limitOfMemory);
  fclose(file);
  return 0;
}
