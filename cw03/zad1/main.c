#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char ** argv){
  if(argc!=2){
    printf("Zla ilosc argumentow! Potrzebny jest jeden - nazwa zmiennej srodowskowej.\n");
    exit(1);
  }
  char * nameOfEnv=argv[1];
  nameOfEnv[strlen(nameOfEnv)-1]='\0';
	if(getenv(nameOfEnv)==NULL) printf("Nie istnieje podana zmienna srodowiskowa w srodowisku programistycznym.\n");
	else printf("Zmienna srodowiskowa %s: %s.\n",nameOfEnv,getenv(nameOfEnv));
	return (0);
}
