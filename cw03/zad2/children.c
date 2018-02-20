#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char ** argv){
	printf("Argumenty, które posiada proces potomny: \n");
	for(int i=0 ; i<argc ; i++) printf("%s ",argv[i]);
	printf("\n");
  return 0;
}
