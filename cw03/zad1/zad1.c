#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

static const int MAX=5;
pid_t children;

void interpret(FILE * file){
  char * nameOfVariable;
  char * valueOfVariable;
  char * charactersToOmit=" #\n";
  char * tableOfArg[MAX];
  char * line=NULL;
  size_t lengthOfLine=0;
  int check;
  while(getline(&line,&lengthOfLine,file)!=(-1)){
    if(line[0]=='\n') continue;
    if(line[0]=='#'){
      int iter=0;
  		while(line[iter]!='\0' && line[iter]!=' ') iter++;
  		if(line[iter]=='\0'){
				nameOfVariable=strtok(line,charactersToOmit);
				if(getenv(nameOfVariable)==NULL) printf("Zmienna %s nie moze zostac usunieta, poniewaz nie istnieje.\n",nameOfVariable);
				else{
					if(unsetenv(nameOfVariable)==-1) printf(" Blad! Usuwanie zmiennej %s nie powiodlo sie.\n",nameOfVariable);
					else printf("Zmienna %s zostala usunieta.\n",nameOfVariable);
  			}
  		}
      else{
  			nameOfVariable=strtok(line,charactersToOmit);
  			valueOfVariable=strtok(NULL,charactersToOmit);
  			if(getenv(nameOfVariable)==NULL){
					if(setenv(nameOfVariable,valueOfVariable,0)==(-1)) printf(" Blad! Ustawianie zmiennej %s nie powiodlo sie.\n",nameOfVariable);
					else printf("Zmienna %s zostala ustawiona.\n",nameOfVariable);
  			}
  			else{
  				if(setenv(nameOfVariable,valueOfVariable,1)==(-1)) printf(" Blad! Ustawianie zmiennej %s nie powiodlo sie.\n",nameOfVariable);
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
        printf(" Blad! Nie powiodl sie fork.\n");
        exit(1);
      }
      else if(children==0){
  			printf("%s--%s",tableOfArg[0],tableOfArg[1]);
  			if(execvp(nameOfVariable,tableOfArg)==(-1)){
  				printf(" Blad! Nie powiodl sie execvp.\n");
  				exit(1);
  			}
			}
			else{
  			wait(&check);
  			if(!WIFEXITED(check)) printf("Proces %s zostal zakonczony bledem %d.\n",nameOfVariable,WEXITSTATUS(check));
			}
    }
  }
}

int main(int argc, char ** argv){
  if(argc!=2){
    printf("Zla ilosc argumentow! Potrzebny jest jeden - sciezka do pliku.\n");
    exit(1);
  }
  char * fileName=argv[1];
  FILE * file=fopen(fileName,"r");
	if(file==NULL){
		printf(" Blad! Nie udalo otworzyc sie pliku.\n");
		exit(1);
	}
  interpret(file);
  fclose(file);
  return 0;
}
